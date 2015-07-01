# WORM-FS

WORM-FS is WORM (Write Once Read Many) compliant file system. When a file is created, you cannot delete or update it, until the retention period has expired.

You can set up retention periods at file level using filters. For instance, you can choose to set retention period of 1 year for png files, retention period of 6 month for wav files etc... If no filter applies to one file/folder, the parent folder retentions is used. For this reason you can set hierarchical structures, for instance one root folder with 5 years retention, another one with 2 years retention etc... 


Please note that retention period is defined in days.

