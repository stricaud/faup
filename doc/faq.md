## Faup Asked Questions

#### Q: When I run faup, I have a shared library load error, such as:

    $ faup
    faup: error while loading shared libraries: libfaupl.so.1: cannot open shared object file: No such file or directory

#### A: On Linux, just make sure you have the path where faup in installed (default prefix is /usr/local/) and run ldconfig. For example:

    # echo "/usr/local/lib" > /etc/ld.conf.d/local.conf
    # ldconfig

#### Q: With the Splunk addon, I have this:

    /opt/splunk/etc/apps/faup/opt/../opt/linux-x86_64/bin/faup: /lib64/libc.so.6: version `GLIBC_2.11' not found (required by /opt/splunk/etc/apps/faup/opt/../opt/linux-x86_64/lib/liblua-5.1.so)
    /opt/splunk/etc/apps/faup/opt/../opt/linux-x86_64/bin/faup: /lib64/libc.so.6: version `GLIBC_2.7' not found (required by /opt/splunk/etc/apps/faup/opt/../opt/linux-x86_64/lib/liblua-5.1.so)
 
#### Q: Install Faup from git, then edit etc/apps/faup/bin/faup.py and replace:

    def where_is_faup():
        if platform.system() == "Darwin":
                return os.environ['SPLUNK_HOME'] + "/etc/apps/faup/opt/faup-darwin"
        if platform.system() == "Linux":
                return os.environ['SPLUNK_HOME'] + "/etc/apps/faup/opt/faup-linux"
        # I don't know, so let's trust the system                                                                                                            
        return "faup"
        
#### With:

    def where_is_faup():
      return "faup" 
      
