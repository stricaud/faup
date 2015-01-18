## Faup Library API

The Faup library is designed to be embeded with no pain. The library is lightweight and the license is very permissive, so you can do whatever you want to. We just wanted to fix the url normalization problem and make that available for the general public, whatever you want to do with it.

A great way to see how the C library can be used is to look at the [faup][fauptoolsrc] command line tool source code. It is a minimalistic program, first class citizen of the library. Every feature the library has can be activated for the command line.

### Architecture

Faup architecture is pretty straightforward:

<p align="center"><img src="images/faup-arch.png"/></p>

Of course, speed is what we had in mind all the time. The mozilla.tlds file is loaded at initialization time only.

### Python API

[Click here][pythonapi] to access the documentation on the Python API.

### C API

```C
       faup_handler_t *fh;
       faup_options_t *options;

       options = faup_options_new();
       if (!options) {
       	  fprintf(stderr, "Error: cannot allocate faup options!\n");
       }

       fh = faup_init(options);
       faup_decode(fh, "https://wallinfire.net", strlen("https://wallinfire.net"));
       tld_pos = faup_get_tld_pos(fh); /* will return 19 */       
       tld_size = faup_get_tld_size(fh); /* will return 3 */       
       faup_show(fh, ',', stdout);

       faup_terminate(fh);
```

C++ example (usefull for multithreading)

```C++
       #include <iostream>
       
       #include <faup/faup.h>
       #include <faup/decode.h>
       #include <faup/options.h>
       #include <faup/output.h>
       
       using namespace std;
       
       int main()
       {
         // faup_options_new() is not thread safe, and should only be runned once per code, it is also the part that loads the cached publicsuffix.org file
         // faup_tld_update(); // updates the cached publicsuffix.org file
         faup_options_t *faup_opts;
         faup_opts = faup_options_new();
         // no need for the default csv output
         faup_opts->output = FAUP_OUTPUT_NONE;
         // modules slow down faup_init(), should be loaded only when needed 
         faup_opts->exec_modules = FAUP_MODULES_NOEXEC;
        
         for(int i=0; i<1000000; i++)
         {
           // fh is a pointer to a c struct that has, among others, all the positions where the uri splits in host, tld, etc.
           faup_handler_t *fh;
          
           // check if the options are set
           if (!faup_opts) {
              fprintf(stderr, "Error: cannot allocate faup options!\n");
           }
       
           // init the faup handler
           fh = faup_init(faup_opts);
       
           //the url to parse
           string url = "https://domain.co.uk";
       
           // this is the only command you need to run, to parse a url (if you are not running in multithreaded mode)
           faup_decode(fh, url.c_str(), url.size() );
       
           // equivalent to url.substr(fh->faup.features.tld.pos, fh->faup.features.tld.size);
           string tld = url.substr(faup_get_tld_pos(fh),faup_get_tld_size(fh));
       
           // cleanup
           faup_terminate(fh);
         }
         return 0;
       }
```
For simple projects you can compile with
g++ -W -Wall "faup_simple_example.cpp" -o "faup_simple_example" -lfaupl


### Modules

Faup library supports LUA written modules. It is a convenient way to do a couple of things one may want to do on every given url before and after the parsing.
Internet Explorer emulation is an example of module. Modules are activated only if Lua is installed and if they are enabled. The execution order is given by its
alphabetical name.

For example, if you want to have a module that transforms every given url as uppercase, you can do:

    $ mkdir ~/.faup/modules_available
    $ mkdir ~/.faup/modules_enabled
    $ cat > ~/.faup/modules_available/uppercase.lua << EOF
    > function faup_url_in(url)
    >     local new_url = string.upper(url)
    >     return new_url
    > end
    > EOF
    $ ln -s ~/.faup/modules_available/uppercase.lua ~/.faup/modules_enabled/uppercase.lua
    $ faup www.example.com
    ,,WWW,EXAMPLE.COM,EXAMPLE,WWW.EXAMPLE.COM,COM,,,,

By default, modules are in your share directory: /usr/local/share/faup/ or /usr/share/faup/. This can be overridden if a local ~/.faup/modules_enabled/ has the wanted module. 

The output can also by controled in the same script if the function exists. For example:

    function faup_output(orig_url, positions_and_sizes)
    	     local new_positions_and_sizes = {numfields=1}

	     for k,v in pairs(positions_and_sizes) do
	     	 new_positions_and_sizes.numfields = new_positions_and_sizes.numfields + 1
		 new_positions_and_sizes[tostring(k)] = v
	     end

	     return orig_url, new_positions_and_sizes
    end



[fauptoolsrc]: http://github.com/stricaud/faup/blob/master/src/tools/faup.c
[pythonapi]: python-api.md
