# Experimental MakeCode extension to run ML4F models

[![MakeCode Project](https://github.com/microbit-foundation/pxt-ml-runner-poc/actions/workflows/makecode.yml/badge.svg)](https://github.com/microbit-foundation/pxt-ml-runner-poc/actions/workflows/makecode.yml)
[![Header Generator Tests](https://github.com/microbit-foundation/pxt-ml-runner-poc/actions/workflows/header-gen.yml/badge.svg)](https://github.com/microbit-foundation/pxt-ml-runner-poc/actions/workflows/header-gen.yml)

This project includes [ML4F](https://github.com/microsoft/ml4f) and a wrapper
to invoke a known type of model. It is left slim to be able to be import it
in other MakeCode extensions and a MicroPython module.


## Developer considerations

- Almost nothing implemented yet
- Currently it embeds a model in a C file as an array with blob data
- Due to the model size, BLE had to be disabled
    - This currently doesn't work on live MakeCode, only beta
        - Actually it doesn't look like it works in beta either, just locally
    - Adding other BLE extensions might fail
- This repository [issue tracker](https://github.com/microbit-foundation/pxt-ml-runner-poc/issues/)
  contains issues that could be encountered and workarounds 
- Only built and tested on micro:bit V2


## Use as Extension

This repository can be added as an **extension** in MakeCode.

* Open [MakeCode beta](https://makecode.microbit.org/beta)
* Click on **New Project**
* Click on **Extensions** under the gearwheel menu
* Search for **https://github.com/microbit-foundation/pxt-ml-runner-poc** and import


## Edit this project

### In MakeCode online editor

To edit this repository in MakeCode.

* Open [MakeCode](https://makecode.microbit.org)
* Click on **Import** then click on **Import URL**
* Paste **https://github.com/microbit-foundation/pxt-ml-runner-poc** and click import

### Building locally

Ensure you have the required toolchain to build for V1 and V2
(arm-none-eabi-gcc, python, yotta, cmake, ninja, srec_cat) or docker.

```bash
git clone https://github.com/microbit-foundation/pxt-ml-runner-poc
cd pxt-ml-runner-poc
npm install pxt --no-save
npx pxt target microbit --no-save
npx pxt install
PXT_FORCE_LOCAL=1 PXT_NODOCKER=1 npx pxt
```

For the V1 build Yotta can hit the GitHub rate limits quite easily if the
project is built from a clean state more than once.
A V2-only build can be performed with the `PXT_COMPILE_SWITCHES=csv---mbcodal`
environmental variable.

```
PXT_FORCE_LOCAL=1 PXT_NODOCKER=1 PXT_COMPILE_SWITCHES=csv---mbcodal npx pxt
```

## Build flags

To enable debug print from this extension, add the following into your
pxt.json file:

```json
{
    "yotta": {
        "config": {
            "ML_DEBUG_PRINT": 1
        }
    }
}
```


## License
This software is under the MIT open source license.

[SPDX-License-Identifier: MIT](LICENSE)


## Code of Conduct

Trust, partnership, simplicity and passion are our core values we live and
breathe in our daily work life and within our projects. Our open-source
projects are no exception. We have an active community which spans the globe
and we welcome and encourage participation and contributions to our projects
by everyone. We work to foster a positive, open, inclusive and supportive
environment and trust that our community respects the micro:bit code of
conduct. Please see our [code of conduct](https://microbit.org/safeguarding/)
which outlines our expectations for all those that participate in our
community and details on how to report any concerns and what would happen
should breaches occur.


#### Metadata (used for search)

* for PXT/microbit
