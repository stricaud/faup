## Faup Library API

The Faup library is designed to be embeded with no pain. The library is lightweight and the license is very permissive, so you can do whatever you want to. We just want to fix the url normalization problem.

A great way to see how the C library can be used is to look at the [faup][fauptoolsrc] command line tool source code.

### Architecture

[ URL ] -> [ Features discovery ] -> [ Decoding ] -> [ URL Fields ]


### Python API

```python
       >>> from pyfaup.faup import Faup
       >>> f = Faup()
       >>> f.decode("https://www.slashdot.org")
       >>> f.get()
       {'credential': None, 'domain': 'slashdot.org', 'subdomain': 'www', 'fragment': None, 'host': 'www.slashdot.org', 'resource_path': None, 'tld': 'org', 'query_string': None, 'scheme': 'https', 'port': None}
       >>> 
```

```python
from pyfaup.faup import Faup
import sys
import codecs

fp = Faup()
file_urls=codecs.open(sys.argv[1],'r','ascii',errors='ignore')
urls=file_urls.readlines()

for url in urls:
    url=url.replace('\n','')
    print("URL:[%s]" % (url))
    f.decode(url)
    print("-----> Extracted TLD:%s" % f.get_tld())

```


### C API


```C
       faup_handler_t *fh;

       fh = faup_init();
       faup_decode(fh, "https://wallinfire.net", strlen("https://wallinfire.net"));
       tld_pos = faup_get_tld_pos(fh); /* will return 19 */       
       tld_size = faup_get_tld_size(fh); /* will return 3 */       
       faup_show(fh, ',', stdout);

       faup_terminate(fh);
```

[fauptoolsrc]: http://github.com/stricaud/faup/blob/master/src/tools/faup.c

