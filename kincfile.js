let project = new Project('metac');

project.kore = false;
project.cmd = true;
project.addIncludeDir('Libraries/tinycc');
project.addIncludeDir('Libraries/stb');
project.addIncludeDir('Sources');
project.addFiles('Libraries/tinycc/libtcc.c');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
