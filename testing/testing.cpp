
#include <testing.hpp>
#include <game/KMP/Controller.hpp>

KMP::KTPTHolder *SecondaryKTPT(KMP::Controller *controller, u32 idx){
    KMP::KTPTHolder *holder = controller->GetKTPTHolder(1);
    if(holder == NULL) holder = controller->GetKTPTHolder(0);
    return holder;
}
kmCall(0x807ea670, SecondaryKTPT);