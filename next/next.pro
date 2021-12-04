TEMPLATE = subdirs

SUBDIRS= rtk_analysis \
#    rtk_record  \
    str2str_qt     \
    SPP


linux{
    SUBDIRS+= rtkrcv_qt\
}
