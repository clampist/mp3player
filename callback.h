#include "interface.h"

#ifndef CALLBACK_H_
#define CALLBACK_H_
void on_btVolume_clicked(GtkWidget *btVolume,
		                   InterFace *ui);
void on_btPlaylist_clicked(GtkWidget *btPlaylist,
                           InterFace *ui);
void on_btMedialib_clicked(GtkWidget *btMedialib,
                           InterFace *ui);

void on_tlbAdd_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_tlbAdddir_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_tlbDel_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_tlbDelall_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_tlbSearch_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_tlbSendto_clicked(GtkWidget *btMedialib,
                       InterFace *ui);
void on_btFind_clicked(GtkWidget *btMedialib,
                       InterFace *ui);

void on_tbtAdd_clicked(GtkWidget *tbtAdd,
                       InterFace *ui);

void on_tbtOpen_clicked(GtkWidget *tbtOpen,
                       InterFace *ui);

void on_tbtSave_clicked(GtkWidget *tbtSave,
                       InterFace *ui);

void on_tbtAdddir_clicked(GtkWidget *tbtAdddir,
                          InterFace *ui);

void on_tbtDel_clicked(GtkWidget *tbtDel,
                       InterFace *ui);

void on_tbtDelall_clicked(GtkWidget *tbtDelall,
                       InterFace *ui);

void on_cbbMode_active(GtkWidget *cbbMode,
							InterFace *ui);

#endif /* CALLBACK_H_ */
