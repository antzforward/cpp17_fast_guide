#include "LabContext.h"
#include "LabObject.h"
#include "Delegates/Delegate.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UnrealType.h"

int32 RunExperiment(FLabContext& Lab)
{
    if (!Lab.Require(IsInGameThread(), TEXT("UObject lifecycle experiment runs on the game thread")))
        return 1;

    ULabObject* Object = NewObject<ULabObject>(GetTransientPackage());
    if (!Lab.Require(Object != nullptr, TEXT("NewObject created a real UObject"))) return 1;
    Object->AddToRoot();
    const FIntProperty* Property = FindFProperty<FIntProperty>(ULabObject::StaticClass(), GET_MEMBER_NAME_CHECKED(ULabObject, SampleValue));
    Lab.Require(Property && Property->GetPropertyValue_InContainer(Object) == 42,
        TEXT("UHT-generated reflection exposes the actual property value"));
    TWeakObjectPtr<UObject> Weak(Object);
    int32 CallbackCount = 0;
    FSimpleMulticastDelegate Signal;
    // The delegate stores a weak owner. The callback does not capture Object.
    Signal.AddWeakLambda(Object, [&CallbackCount] { ++CallbackCount; });

    CollectGarbage(RF_NoFlags, true);
    Lab.Require(Weak.IsValid(), TEXT("rooted UObject survives a full garbage collection"));
    Signal.Broadcast();
    Lab.Require(CallbackCount == 1, TEXT("weak delegate invokes while its owner is alive"));
    Lab.Metric(TEXT("callbacks_before_gc"), CallbackCount);

    Object->RemoveFromRoot();
    Object = nullptr; // A bare local pointer is not a GC-tracked owning reference.
    CollectGarbage(RF_NoFlags, true);
    Lab.Require(!Weak.IsValid(), TEXT("unrooted UObject with no strong references was collected"));
    Signal.Broadcast();
    Lab.Require(CallbackCount == 1, TEXT("weak delegate skips its collected owner"));
    Lab.Metric(TEXT("callbacks_after_gc"), CallbackCount);
    Lab.Metric(TEXT("weak_valid_after_gc"), Weak.IsValid() ? 1.0 : 0.0);
    Lab.Note(TEXT("This uses real UE garbage collection and weak delegates. No freed pointer is dereferenced, and no address or object-slot reuse is assumed."));
    return Lab.Failures == 0 ? 0 : 1;
}
