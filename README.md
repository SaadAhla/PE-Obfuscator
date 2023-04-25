# PE-Obfuscator
PE obfuscator with Evasion in mind

## Video:
https://user-images.githubusercontent.com/123980007/234171901-c1eb735e-ceca-481d-9310-02ba30f6836b.mp4


### The Obfuscator :
    - Gets Fileless PE from a remote server
    - Drop the Loader in the disk 
    - Add random section to that Loader
    - Add the Fileless PE to the new created Loader section

### The Loader :
    - Unhook ntdll from knowndlls
    - Drop RTCore64 to the disk
    - Load/Install RTCore64
    - Exploit RTCore64 to Remove Kernel Callbacks
    - Map/Load from the added Section
    - Stomped a big module that fit the PE.
    
 ## Credits :
 https://br-sn.github.io/Removing-Kernel-Callbacks-Using-Signed-Drivers/
 https://github.com/br-sn/CheekyBlinder
 https://github.com/lawiet47/STFUEDR
 https://papers.vx-underground.org/papers/Windows/Infection/2015-03-06%20-%20PE%20Infection%20-%20Add%20a%20PE%20section%20-%20with%20code.txt
