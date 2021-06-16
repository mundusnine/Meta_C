const path = require('path');
let project = new Project('metac');

project.kore = false;
project.cmd = true;
project.cmdArgs = [
    '-cust',
    path.resolve(process.cwd(),'samples/data.h'),
    path.resolve(process.cwd(),'include/metac.h'),
    path.resolve(process.cwd(),'include/custom_layer.h')
];
project.addIncludeDir('Libraries/tinycc');
project.addIncludeDir('Libraries/stb');
project.addIncludeDir('Sources');
project.addIncludeDir('include');
project.addFiles('Libraries/tinycc/libtcc.c','samples/custom_layer.c');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
