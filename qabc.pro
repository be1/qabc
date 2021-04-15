include(common.pri)
TEMPLATE = subdirs
SUBDIRS += abcm2ps abc2smf app
abcm2ps.subdir = abcm2ps
abc2smf.subdir = abc2smf
app.subdir = app
app.depends = abcm2ps abc2smf
