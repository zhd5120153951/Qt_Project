#-------------------------------------------------
#
# Project Group created by Zhangsk 2012-04-25
#
#-------------------------------------------------
TEMPLATE = subdirs

SUBDIRS += \
  GLDCommon.pro \
  GLDCrypt.pro \
  GLDZip.pro \
  GLDXML.pro \
  GLDWidget.pro \
  GLDTableView.pro \
  GLDMask.pro

GLDCrypt.depends = GLDCommon
GLDZip.depends = GLDCommon
GLDXML.depends = GLDCommon
GLDWidget.depends = GLDCommon
GLDTableView.depends = GLDCommon GLDWidget GLDXML
  
