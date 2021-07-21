#include "entity.h"

namespace redtea {
namespace core{
    
void Entity::Active() {
    mState = State::Active;
}

void Entity::Deactive() {
    mState = State::Deactive;
}

}
}
