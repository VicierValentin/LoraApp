/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdio>

#include "capteur.h"
#include "lorawanApp.h"

/**
 * Entry point for application
 */

extern sixtron::O2SMPB02E capteur;

int main(void)
{
    LorawanApp loraApp;
    loraApp.lorawanInit();
    capteurInit();

    while (true) {
        capteurSend();
        loraApp.send_message();
        loraApp.receive_message();
    }
}

// EOF
