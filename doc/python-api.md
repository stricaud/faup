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

