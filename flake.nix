{
  description = "Advanced Proximal Optimization Toolbox";

  inputs = {
    gepetto.url = "github:gepetto/nix";
    flake-parts.follows = "gepetto/flake-parts";
    systems.follows = "gepetto/systems";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { lib, ... }:
      {
        systems = import inputs.systems;
        imports = [
          inputs.gepetto.flakeModule
          {
            flakoboros = {
              extraDevPyPackages = [ "proxsuite" ];
              overrideAttrs.proxsuite = _: {
                src = lib.fileset.toSource {
                  root = ./.;
                  fileset = lib.fileset.unions [
                    ./benchmark
                    ./bindings
                    ./cmake-external
                    ./CMakeLists.txt
                    ./doc
                    ./examples
                    ./include
                    ./package.xml
                    ./test
                  ];
                };
                postPatch = "";
              };
              extends.eigen5 = final: prev: {
                eigen = final.eigen_5;
                pythonPackagesExtensions = prev.pythonPackagesExtensions ++ [
                  (python-final: python-prev: {
                    scipy = python-prev.scipy.overrideAttrs {
                      # broken on linux arm
                      doInstallCheck = false;
                    };
                  })
                ];
              };
            };
          }
        ];
      }
    );
}
