# Experimental MakeCode extension to run ML models with ML4F

This project includes [ML4F](https://github.com/microsoft/ml4f) and a wrapper
to invoke a known type of model. It is left slim to be able to be import it
in other MakeCode extensions and a MicroPython module.

## Developer considerations

- Almost nothing implemented yet
- Currently it embeds a model in a C file as an array with blob data
- Due to the model size, BLE had to be disabled
    - This currently doesn't work on live MAkeCode, only beta
        - Actually it doesn't look like it works in beta either, just locally
    - Adding other BLE extensions might fail
- Only built and tested on micro:bit V2

## Use as Extension

This repository can be added as an **extension** in MakeCode.

* open [MakeCode](https://makecode.microbit.org)
* click on **New Project**
* click on **Extensions** under the gearwheel menu
* search for **https://github.com/microbit-foundation/pxt-ml-runner-poc** and import

## Edit this project

### In MakeCode online editor

To edit this repository in MakeCode.

* open [MakeCode](https://makecode.microbit.org)
* click on **Import** then click on **Import URL**
* paste **https://github.com/microbit-foundation/pxt-ml-runner-poc** and click import


## Building locally

```bash
git clone https://github.com/microbit-foundation/pxt-ml-runner-poc
cd pxt-ml-runner-poc
npm install pxt --no-save
npx pxt target microbit --no-save
cp node_modules/pxtcli.json ./pxtcli.json
npm install pxt-microbit@6.1.10 --no-save
mv ./pxtcli.json node_modules/pxtcli.json
npx pxt install
PXT_FORCE_LOCAL=1 PXT_NODOCKER=1 PXT_COMPILE_SWITCHES=csv---mbcodal npx pxt
```

> ![WARNING]
> Forcing microbit target at version 6.1.10 due to this issue:
> https://github.com/microsoft/pxt-microbit/pull/5481


#### Metadata (used for search, rendering)

* for PXT/
<script src="https://makecode.com/gh-pages-embed.js"></script><script>makeCodeRender("{{ site.makecode.home_url }}", "{{ site.github.owner_name }}/{{ site.github.repository_name }}");</script>
