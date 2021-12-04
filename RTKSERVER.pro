TEMPLATE = subdirs

SUBDIRS= src \
         app \
         korolib \
         next \
#         learn

app.depends = src
