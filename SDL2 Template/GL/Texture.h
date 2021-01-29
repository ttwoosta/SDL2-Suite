//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include "OpenGL.h"

namespace gl {
    
    class Texture: public Name<Texture> {
    public:
        class Unit {
        public:
            using Index = Int;
        };

        Texture() = default;
        
        template<typename T>
        Texture(const T& source) : Texture{} { Load(source); }
        
        template <typename T>
        Texture& Load(const T& source);
        
        Unit::Index Activate(Unit::Index index = 0) const;
        
        static Unit::Index Deactivate(Unit::Index index = 0);

		static void init();
    };
}
