
# How to translate 2048 to another language.

First, if you do not have Gettext, get it by entering the following command by terminal

```
sudo apt-get install gettext
```

We aim to have the following structure for our directory:

./po/language_name/file.po

So, go to the ./po directory and then create a directory with your language standard name. 

for example:

```
mkdir po/es_AR
```

The next step is to generate the .po files with the following command (change 'es_AR' by your language standard name):
```
msginit --input=po/gfx_curses.pot --locale=es_AR --output=po/es_AR/gfx_curses.po
msginit --input=po/gfx_terminal.pot --locale=es_AR --output=po/es_AR/gfx_terminal.po
msginit --input=po/highscore.pot --locale=es_AR --output=po/es_AR/highscore.po
```

### Translation

Now, we can translate the strings:

so, go to ./po/language_name/ and translate the strings in the .po files with a text editor.

for example:
![Screenshot](https://k62.kn3.net/5/0/D/B/4/4/870.png)


Nwe should create the directory where we'll place the .mo files (change es_AR by your language standard name):

```
mkdir -p i18n/es_AR/LC_MESSAGES
```

The last step is to generate the .mo file:

```
msgfmt --output-file=i18n/es_AR/LC_MESSAGES/gfx_curses.mo po/es_AR/gfx_curses.po
msgfmt --output-file=i18n/es_AR/LC_MESSAGES/gfx_terminal.mo po/es_AR/gfx_terminal.po
msgfmt --output-file=i18n/es_AR/LC_MESSAGES/highscore.mo po/es_AR/highscore.po
```

## And this is it!
