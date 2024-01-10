# How to add translation to the project

First, you can enter the language dir in the ```locales``` dir.If you want to add a new language.Please follow the following structure:

```locales/language_[region[.encoding]]/LC_MESSAGES```

For example,```locales/en_US.UTF-8/LC_MESSAGES```,```locales/en_US/LC_MESSAGES```,```locales/en/LC_MESSAGES```.

Then,create a translation file named ```trans.po``` in the dir.

Please refer to the [Translation Reference](./reference.md) to study how to add translation in the ```.po``` file.

After you finished the ```.po``` file.You can compile it to the binary file ```.mo``` that the Trc finally use.You can simply run the python script ```script/gen_locales.py``` to rebuilt all the language resources files.You can also run ```msgfmt -o trans.mo trans.po``` to jsut built this file.The former way is a little slower,but is more easy for beginners.

If there isn't ```msgfmt``` command in you Operating System.You can install it by the package manager or you can use ```msgfmt.py``` in python tools.It can work greatly,too.

Thank you for your contribution!
