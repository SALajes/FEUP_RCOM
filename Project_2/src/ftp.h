#pragma once

#include "url.h"

struct ftp{
    int control_fd;
    int data_fd;
};

int connectHost(struct ftp* ftp, struct url * url);
int loginUser(struct ftp * ftp, struct url * url);
int changeDirectory(struct ftp * ftp, struct url * url);
int passiveMode(struct ftp * ftp);
int retrive(struct ftp* ftp, struct url * url);
int download(struct ftp* ftp, struct url * url);
int disconnect(struct ftp* ftp);
