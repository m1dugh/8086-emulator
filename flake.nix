{
  description = "An intel 8086 emulator.";

  outputs = {
      self,
      nixpkgs,
      ...
  }: 
  let 
    inherit (nixpkgs) lib;
    name = "mmvm";
    supportedSystems = [
      "x86_64-linux"  
    ];
    forAllSystems = lib.genAttrs supportedSystems;
    nixpkgsFor = forAllSystems(system: import nixpkgs {
        inherit system;
    });
  in {

    packages = forAllSystems(system: 
    let pkgs = nixpkgsFor.${system};
    inherit (pkgs) stdenv;
    in {
        emulator = stdenv.mkDerivation {
            name = "emulator";
            src = ./.;

            buildInputs = with pkgs; [
                gcc
                gnumake
            ];

            configurePhase = ''
                mkdir -p $out/bin
                '';

            buildPhase = ''
                make prod
            '';

            installPhase = ''
                install -D ./bin/Release/mmvm -m 0555 "$out/bin/${name}"
            '';

        };

        default = self.packages.${system}.emulator;
    });

    apps = forAllSystems(system: 
    let 
        pkgs = nixpkgsFor.${system};
        local_pkgs = self.packages.${system};
    in {
        emulator = {
            type = "app";
            program = with local_pkgs; "${emulator}/bin/${name}";
        };

        default = self.apps.${system}.emulator;
    });
  };
}
