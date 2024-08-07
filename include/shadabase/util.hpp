// 
// Copyright (c) 2024 laf163@gmail.com
// This file is part of the shadabase library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//

#pragma once

#define TO_STR(value) #value
#define TO_STR2(value) TO_STR(value)

// usage: #pragma todo("xxx")
#define todo(msg) message(__FILE__ "(" TO_STR2(__LINE__) "): TODO " msg)