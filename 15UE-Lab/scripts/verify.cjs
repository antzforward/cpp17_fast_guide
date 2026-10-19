'use strict';
// Exercise the actual UE workflow, including expected failures. Never edit learner files.
const fs=require('node:fs');
const path=require('node:path');
const cp=require('node:child_process');
const root=path.resolve(__dirname,'..');
const cli=path.join(__dirname,'ue-lab.cjs');
const cases=[
  {args:['run','probe']},
  {args:['run','array','--set','Count=2','--set','Reserve=0','--set','Shrink=1']},
  {args:['run','array','--set','Count=10000','--set','Reserve=1','--set','Shrink=0']},
  {args:['run','lifetime']},
  {args:['run','flow','--set','Count=4096']},
  {args:['run','transport','--set','Count=129','--set','BatchSize=64','--set','Quantum=10']},
  {args:['run','transport','--set','Count=1','--set','BatchSize=1','--set','Compression=0']},
  {args:['check','01_array_parameters','--solution']},
  ...[1,2,3].map(level=>({args:['check','01_array_parameters','--solution','--level',String(level)]})),
  {args:['run','array','--set','Count=-1'],expected:/UELAB_CHECK FAIL Count must/},
  {args:['run','transport','--param','BatchSize=16,64'],expected:/only valid for sweep/},
  {args:['sweep','transport','--set','BatchSize=64','--param','batchsize=16,32'],expected:/Duplicate parameter/},
  {args:['run','transport','--set','Count=1,2'],expected:/takes one value/},
];
// Once a learner has supplied implementations, an expected linker failure is no longer appropriate.
const answer=fs.readFileSync(path.join(root,'lessons','01_array_parameters','array_parameters.cpp'),'utf8');
if(!/\{/.test(answer.replace(/\/\/.*$/gm,''))) cases.push({args:['check','01_array_parameters','--level','1'],expected:/LNK2019[\s\S]*BuildSequentialBatch/});
cases.push({args:['run','probe']});
const reports=[];
for(const item of cases) {
  const result=cp.spawnSync(process.execPath,[cli,...item.args],{cwd:root,encoding:'utf8',timeout:720000,maxBuffer:32*1024*1024,windowsHide:true});
  const log=(result.stdout||'')+'\n'+(result.stderr||'');
  const passed=!result.error && (item.expected?result.status!==0&&item.expected.test(log):result.status===0&&/UELAB_RESULT status=PASS/.test(log));
  const title=item.args.join(' ');
  console.log(`${passed?'PASS':'FAIL'} ${title}${item.expected?' (expected failure verified)':''}`);
  reports.push({command:title,passed,expectedFailure:!!item.expected,exitCode:result.status,error:result.error?.message||null});
  if(!passed) { console.log(log); break; }
}
fs.mkdirSync(path.join(root,'artifacts'),{recursive:true});
fs.writeFileSync(path.join(root,'artifacts','verification.json'),JSON.stringify({date:new Date().toISOString(),passed:reports.every(x=>x.passed)&&reports.length===cases.length,checks:reports},null,2));
if(reports.some(x=>!x.passed)||reports.length!==cases.length) process.exitCode=1;
