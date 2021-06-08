let project = new Project('metac');

project.kore = false;
project.cmd = true;
project.addIncludeDir('Libraries/tinycc');
project.addIncludeDir('Libraries/stb');
project.addIncludeDir('Sources');
project.addIncludeDir('include');
project.addFiles('Libraries/tinycc/libtcc.c','samples/custom_layer.c');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
