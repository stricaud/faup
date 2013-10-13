## Faup Asked Questions

Q: When I run faup, I have a shared library load error, such as:

   
   $ faup
   faup: error while loading shared libraries: libfaupl.so.1: cannot open shared object file: No such file or directory


A: On Linux, just make sure you have the path where faup in installed (default prefix is /usr/local/) and run ldconfig. For example:


   # echo "/usr/local/lib" > /etc/ld.conf.d/local.conf
   # ldconfig


