{
  description = "ESPHome dev env for Home Assistant Voice Preview Edition (VPE)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
    flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-parts, ... }@inputs: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [ "x86_64-linux" ];

    perSystem = { system, pkgs-dev, lib, ... }: {
      _module.args.pkgs-dev = import nixpkgs {
        inherit system;
      };

      devShells.default = pkgs-dev.mkShell rec {
        name = "ha-vpe";

        packages = with pkgs-dev; [
          esphome
          platformio
          platformio-core
        ];
      };
    };
  };
}
