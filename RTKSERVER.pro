TEMPLATE = subdirs

SUBDIRS= src \
         app \
         next \
         learn

app.depends = src
