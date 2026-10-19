'use strict';

const fs = require('node:fs');
const path = require('node:path');
const cp = require('node:child_process');
const crypto = require('node:crypto');
const root = path.resolve(__dirname, '..');
const readJson = p => JSON.parse(fs.readFileSync(p, 'utf8').replace(/^\uFEFF/, ''));
const settings = {...readJson(path.join(root, 'settings.json')),
  ...(fs.existsSync(path.join(root, 'settings.local.json')) ? readJson(path.join(root, 'settings.local.json')) : {})};
const engine = path.resolve(process.env.UE_LAB_ENGINE || settings.engineRoot);
const project = path.join(root, 'UELab.uproject');
const generated = path.join(root, 'Source', 'UELab', 'Private');
const output = path.join(root, 'artifacts');
const args = process.argv.slice(2);
let activeRun = {};
let lockOwned = false;
let buildInfo = {};

function fail(message) { throw new Error(message); }
function acquireLock() {
  fs.mkdirSync(output,{recursive:true});
  const lock=path.join(output,'.run.lock');
  if(fs.existsSync(lock)) {
    const previous=readJson(lock);
    let alive=true;
    try { process.kill(previous.pid,0); } catch(error) { if(error.code==='ESRCH') alive=false; }
    if(alive) fail(`Another UE Lab run owns the shared host (PID ${previous.pid}). Wait for it to finish.`);
    fs.unlinkSync(lock);
  }
  const fd=fs.openSync(lock,'wx');
  fs.writeFileSync(fd,JSON.stringify({pid:process.pid,created:new Date().toISOString()}));
  fs.closeSync(fd); lockOwned=true;
}
function releaseLock() {
  if(lockOwned) { fs.unlinkSync(path.join(output,'.run.lock')); lockOwned=false; }
}
process.on('exit',releaseLock);
function writeChanged(file, data) {
  if (fs.existsSync(file) && fs.readFileSync(file, 'utf8') === data) return;
  fs.mkdirSync(path.dirname(file), {recursive: true});
  fs.writeFileSync(file, data, 'utf8');
}
function engineFiles() {
  const dotnetRoot = path.join(engine, 'Engine', 'Binaries', 'ThirdParty', 'DotNet');
  const versions = fs.existsSync(dotnetRoot) ? fs.readdirSync(dotnetRoot).filter(x => /^\d+\./.test(x)).sort((a,b) => b.localeCompare(a, undefined, {numeric:true})) : [];
  const dotnet = versions.map(v => path.join(dotnetRoot, v, 'win-x64', 'dotnet.exe')).find(fs.existsSync);
  return {dotnet, ubt:path.join(engine,'Engine','Binaries','DotNET','UnrealBuildTool','UnrealBuildTool.dll'),
    editor:path.join(engine,'Engine','Binaries','Win64','UnrealEditor-Cmd.exe'),
    version:path.join(engine,'Engine','Build','Build.version')};
}
function doctor() {
  const files = engineFiles();
  let ok = true;
  console.log(`UE Lab: ${root}\nNode: ${process.version}\nEngine: ${engine}`);
  for (const [name, file] of Object.entries(files)) {
    const found = !!file && fs.existsSync(file); ok &&= found;
    console.log(`${found ? 'OK  ' : 'MISS'} ${name}: ${file || '(not found)'}`);
  }
  if (fs.existsSync(files.version)) {
    const v=readJson(files.version); const actual=`${v.MajorVersion}.${v.MinorVersion}.${v.PatchVersion}`;
    console.log(`Engine version: ${actual}; verified baseline: ${settings.expectedEngineVersion}`);
    if (actual !== settings.expectedEngineVersion) console.log('NOTE: another version needs a fresh build and verification.');
  }
  if (!ok) fail('Engine files missing. Set UE_LAB_ENGINE or settings.local.json.');
}
function list() {
  console.log('Examples:');
  for (const file of fs.readdirSync(path.join(root,'examples')).filter(x=>x.endsWith('.cpp'))) console.log(`  ${file.slice(0,-4)}`);
  console.log('Lessons:');
  const folder=path.join(root,'lessons');
  if(fs.existsSync(folder)) for(const file of fs.readdirSync(folder)) if(fs.statSync(path.join(folder,file)).isDirectory()) console.log(`  ${file}`);
}
function parseOptions(input) {
  const options={sets:[],params:[],solution:false,level:0,repeat:1,noBuild:false};
  for(let i=0;i<input.length;i++) {
    const arg=input[i];
    if(arg==='--solution') options.solution=true;
    else if(arg==='--no-build') options.noBuild=true;
    else if(arg==='--set' || arg==='--param') {
      const value=input[++i];
      if(!value || !/^[A-Za-z][A-Za-z0-9_]*=[A-Za-z0-9_.,+\-]+$/.test(value)) fail(`${arg} requires Name=Value (no spaces).`);
      if(arg==='--set' && value.includes(',')) fail('--set takes one value; use sweep --param for a list.');
      options[arg==='--set'?'sets':'params'].push(value);
    } else if(arg==='--level' || arg==='--repeat') {
      const value=Number(input[++i]);
      if(!Number.isInteger(value) || value<0 || (arg==='--level' ? value>3 : value<1 || value>10)) fail(`Invalid ${arg}.`);
      options[arg==='--level'?'level':'repeat']=value;
    } else fail(`Unknown option: ${arg}`);
  }
  const seen=new Set();
  for(const assignment of [...options.sets,...options.params]) {
    const key=assignment.split('=')[0].toLowerCase();
    if(['level','run','project'].includes(key)) fail(`Reserved parameter: ${key}`);
    if(seen.has(key)) fail(`Duplicate parameter: ${key}. Use either --set or --param for this name.`);
    seen.add(key);
  }
  return options;
}
function resolveCase(name) {
  if(!name) fail('A case name or .cpp path is required.');
  const file=name.endsWith('.cpp')?path.resolve(root,name):path.join(root,'examples',name+'.cpp');
  const rel=path.relative(root,file);
  if(rel.startsWith('..') || path.isAbsolute(rel) || !fs.existsSync(file)) fail(`Case not found inside UE Lab: ${name}`);
  return file;
}
function prepareCase(file, exercise, level) {
  let source=fs.readFileSync(file,'utf8');
  if(exercise) {
    writeChanged(path.join(generated,'ActiveExercise.inl'),fs.readFileSync(exercise,'utf8'));
    for (const header of fs.readdirSync(path.dirname(exercise)).filter(x=>x.endsWith('.h'))) {
      writeChanged(path.join(generated,'ActiveLesson',header),fs.readFileSync(path.join(path.dirname(exercise),header),'utf8'));
    }
    source=`#define UELAB_LEVEL ${level}\n`+source;
  }
  writeChanged(path.join(generated,'ActiveExperiment.cpp'),source);
  const hash=crypto.createHash('sha256').update(source);
  if(exercise) {
    hash.update(fs.readFileSync(exercise));
    for(const header of fs.readdirSync(path.dirname(exercise)).filter(x=>x.endsWith('.h')).sort()) hash.update(fs.readFileSync(path.join(path.dirname(exercise),header)));
  }
  activeRun={sourceFile:path.relative(root,file),exerciseFile:exercise?path.relative(root,exercise):null,
    variant:exercise?(path.basename(exercise).startsWith('solution_')?'solution':'learner'):'example',sourceSha256:hash.digest('hex')};
}
function build() {
  const files=engineFiles();
  if(!files.dotnet || !fs.existsSync(files.ubt)) fail('Missing UBT/runtime. Run ue.bat doctor.');
  const env={...process.env, DOTNET_ROOT:path.dirname(files.dotnet),DOTNET_MULTILEVEL_LOOKUP:'0'};
  // A shared source path can change twice within the timestamp granularity used by
  // an incremental build (especially after a failed link). Force this ONE object
  // to be rebuilt; leave the host, UHT, shared PCH and engine caches intact.
  const intermediates=path.join(root,'Intermediate','Build','Win64');
  function discardSelectedObject(folder) {
    if(!fs.existsSync(folder)) return;
    for(const item of fs.readdirSync(folder,{withFileTypes:true})) {
      const file=path.join(folder,item.name);
      if(item.isDirectory()) discardSelectedObject(file);
      else if(item.isFile() && path.basename(folder)==='UELab' && item.name==='ActiveExperiment.cpp.obj') fs.unlinkSync(file);
    }
  }
  discardSelectedObject(intermediates);
  console.log('Building the selected experiment (UE engine modules are reused)...');
  const started=Date.now();
  const result=cp.spawnSync(files.dotnet,[files.ubt,'UELabEditor','Win64','Development',`-Project=${project}`,'-WaitMutex','-NoHotReloadFromIDE'],
    {cwd:root,env,encoding:'utf8',maxBuffer:32*1024*1024,timeout:(settings.buildTimeoutSeconds||600)*1000,windowsHide:true});
  const log=(result.stdout||'')+'\n'+(result.stderr||'');
  const buildLog=path.join(output,'build-'+new Date().toISOString().replace(/[:.]/g,'-')+'.log');
  fs.writeFileSync(buildLog,log);
  fs.writeFileSync(path.join(output,'build-last.log'),log);
  buildInfo={durationMs:Date.now()-started,exitCode:result.status,log:buildLog,
    toolchain:log.split(/\r?\n/).find(line=>line.includes('toolchain'))||null};
  console.log(result.status===0 ? log.split(/\r?\n/).filter(line=>/toolchain|Result:|Total execution time:|up to date/i.test(line)).join('\n') : log);
  if(result.error) fail(`Build failed: ${result.error.message}`);
  if(result.status!==0) fail(`Build failed (exit ${result.status}). The previous executable was not run. Log: ${buildLog}`);
}
function run(name,sets,level=0) {
  const files=engineFiles();
  const started=Date.now();
  const result=cp.spawnSync(files.editor,[project,'-run=UELab','-nullrhi','-unattended','-nosound','-NoShaderCompile','-nop4','-nosplash','-stdout','-FullStdOutLogOutput','-UTF8Output',`-Level=${level}`,...sets.map(x=>'-'+x)],
    {cwd:root,encoding:'utf8',timeout:(settings.timeoutSeconds||120)*1000,maxBuffer:32*1024*1024,windowsHide:true});
  const log=(result.stdout||'')+'\n'+(result.stderr||'');
  const metrics={};
  for(const match of log.matchAll(/UELAB_METRIC ([A-Za-z0-9_.-]+)=([-+\d.eE]+)/g)) metrics[match[1]]=Number(match[2]);
  const checks=log.split(/\r?\n/).filter(line=>line.includes('UELAB_CHECK'));
  const passed=!result.error && result.status===0 && /UELAB_RESULT status=PASS/.test(log) && !/UELAB_CHECK FAIL/.test(log);
  const report={case:name,...activeRun,parameters:sets,level,engineRoot:engine,engineVersion:readJson(files.version),build:buildInfo,passed,exitCode:result.status,error:result.error?.message||null,wallMs:Date.now()-started,metrics,checks};
  fs.mkdirSync(output,{recursive:true});
  const stamp=new Date().toISOString().replace(/[:.]/g,'-');
  const base=path.join(output,`${stamp}_${name.replace(/[^A-Za-z0-9_-]/g,'_')}`);
  fs.writeFileSync(base+'.log',log);
  fs.writeFileSync(base+'.json',JSON.stringify(report,null,2));
  fs.writeFileSync(path.join(output,'last.json'),JSON.stringify(report,null,2));
  const relevant=log.split(/\r?\n/).filter(line=>line.includes('UELAB_'));
  console.log(relevant.length ? relevant.join('\n') : log.slice(-6000));
  console.log(`${passed?'PASS':'FAIL'} ${name}; report: ${base}.json`);
  if(!passed) console.log(`Full engine log: ${base}.log${result.error?'\n'+result.error.message:''}`);
  return report;
}
function help() {
  console.log(`Usage (from repository root):
  ue.bat doctor
  ue.bat list
  ue.bat run probe
  ue.bat run array --set Count=10000 --set Reserve=1
  ue.bat run examples/my_experiment.cpp
  ue.bat check 01_array_parameters --solution --level 1
  ue.bat sweep transport --param BatchSize=16,64,256 --param Quantum=1,10
  ue.bat verify

Options: --set Name=Value, --level 0|1|2|3, --solution (check only).
Reports: 15UE-Lab/artifacts/*.json and *.log.
Engine override: UE_LAB_ENGINE or settings.local.json. No editor UI required.`);
}
try {
  const command=args.shift()||'help';
  if(command==='doctor') doctor();
  else if(command==='list') list();
  else if(command==='help' || command==='--help') help();
  else if(command==='verify') {
    const result=cp.spawnSync(process.execPath,[path.join(__dirname,'verify.cjs')],{cwd:root,stdio:'inherit',windowsHide:true});
    process.exitCode=result.status===0?0:1;
  }
  else if(['run','check','sweep'].includes(command)) {
    const name=args.shift(); const options=parseOptions(args);
    if(command!=='check' && options.solution) fail('--solution is only valid for check.');
    if(command!=='sweep' && (options.params.length || options.repeat!==1)) fail('--param and --repeat are only valid for sweep.');
    let file,exercise;
    if(command==='check') {
      if(!/^[A-Za-z0-9_-]+$/.test(name||'')) fail('Invalid lesson name.');
      const folder=path.join(root,'lessons',name);
      file=path.join(folder,'test.cpp');
      if(!fs.existsSync(file)) fail(`Lesson not found: ${name}`);
      const candidates=fs.readdirSync(folder).filter(x=>x.endsWith('.cpp')&&x!=='test.cpp'&&(options.solution?x.startsWith('solution_'):!x.startsWith('solution_')));
      if(candidates.length!==1) fail('Lesson must have one answer and one solution file.');
      exercise=path.join(folder,candidates[0]);
    } else file=resolveCase(name);
    if(options.noBuild) fail('--no-build is intentionally unavailable: the staged source must match the executable.');
    acquireLock();
    prepareCase(file,exercise,options.level); build();
    if(command==='sweep') {
      let grid=[options.sets];
      if(!options.params.length && path.basename(file)!=='transport.cpp') fail('Provide at least one --param for this experiment.');
      const fixedKeys=new Set(options.sets.map(x=>x.split('=')[0].toLowerCase()));
      const params=options.params.length?options.params:['BatchSize=16,64,256','Quantum=1,10'].filter(x=>!fixedKeys.has(x.split('=')[0].toLowerCase()));
      for(const param of params) {
        const [key,values]=param.split('=');
        grid=grid.flatMap(row=>values.split(',').map(v=>[...row,`${key}=${v}`]));
        if(grid.length*options.repeat>100) fail('Sweep limited to 100 runs.');
      }
      const reports=[];
      for(const row of grid) for(let n=0;n<options.repeat;n++) reports.push(run(name,row,options.level));
      const keys=[...new Set(reports.flatMap(r=>Object.keys(r.metrics)))];
      const escape=value=>'"'+String(value).replaceAll('"','""')+'"';
      const rows=[['parameters','passed','wall_ms',...keys],...reports.map(r=>[r.parameters.join(' '),r.passed,r.wallMs,...keys.map(k=>r.metrics[k]??'')])];
      const csv=path.join(output,'sweep.csv'); fs.writeFileSync(csv,'\uFEFF'+rows.map(row=>row.map(escape).join(',')).join('\r\n'));
      fs.writeFileSync(path.join(output,'sweep.json'),JSON.stringify(reports,null,2));
      console.log(`Sweep results: ${csv}`); if(reports.some(r=>!r.passed)) process.exitCode=1;
    } else if(!run(name,options.sets,options.level).passed) process.exitCode=1;
  } else fail(`Unknown command: ${command}`);
} catch(error) { console.error('UE Lab: '+error.message); process.exitCode=1; }
