/*
** syscallent.h for  in /home/otoshigami/Workspace/Epitech/git/AUSP_strace
**
** Made by otoshigami
** Login   <otoshigami@epitech.net>
**
** Started on  Sat May 17 18:58:54 2014 otoshigami
** Last update Sat May 17 18:58:58 2014 otoshigami
*/

#ifndef SYSCALLENT_H
# define SYSCALLENT_H

typedef struct	syscallentry_s
{
  int		argsnumber;
  char*		callname;
}		syscallentry_t;

extern syscallentry_t syscallent[];

#endif /* SYSCALLENT_H */
