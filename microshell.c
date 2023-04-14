#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void	restore(int mode)
{
	int fd;

	if (mode == 2)
		fd = dup(0);
	if (mode == 1)
		dup2(fd, 0);
	if (mode == 0)
		close(fd);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}

char	**parse(char	**pp)
{
	int i = 0;
	char	**rer;

	while (pp[i])
		i++;
	if (i == 0)
		return (NULL);
	rer = malloc((i + 1)  * sizeof(char	*));
	if (!rer)
		return (NULL);
	i = 0;
	while (pp[i])
	{
		rer[i] = pp[i];
		i++;
	}
	rer[i] = NULL;
	return (rer);
}
void	ft_cd(char	**cmd)
{
	int i;

	i = 0;
	while (cmd[i])
			i++;
	if (i != 2)
	{
		ft_putstr_fd("error: cd: bad arguments\n", 2);
		return ;
	}
	if (chdir(cmd[1]))
	{
		ft_putstr_fd("error: cd: cannot change directory to ", 2);
		ft_putstr_fd(cmd[1],2);
		ft_putstr_fd("\n", 2);
		return ;
	}
}
void	exec(char	**ex, char	**en, int op)
{
	int 	fd[2];
	int 	pid;
	
	if (!*ex)
		return ;
	if (!strncmp(ex[0], "cd", 3))
	{
		ft_cd(ex);
		return ;
	}
	if (op == 1)
		pipe(fd);
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("error: fatal\n", 2);
		exit(-1);
	}
	if (!pid)
	{
		if (op == 1)
		{
			dup2(fd[1], 1);
			close(fd[1]);
			close(fd[0]);
		}
		execve(ex[0], ex, en);
		ft_putstr_fd("error: cannot execute ", 2);
		ft_putstr_fd(ex[0], 2);
		ft_putstr_fd("\n", 1);
		exit(-1);
	}
	if (op == 1)
	{
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
	}
	else
	{
		waitpid(pid, 0, 0);
		restore(1);
	}

}
void	exec_cmds(char	**cmd, char	**env)
{
	char	**ex;
	int		op;
	
	op = -1;
	restore(2);
	
	while (1)
	{
		ex = cmd;
		if (!op)
			break ;
		while (*cmd && *cmd[0] != '|' && *cmd[0] != ';')
			cmd++;
		if (!*cmd)
			op = 0;
		else	if (*cmd[0] == '|')
			op = 1;
		else	if (*cmd[0] == ';')
			op = 2;
		if (op)
		{
			*cmd = NULL;	
			cmd++;
		}
		exec(ex, env, op);
	}
}
int main(int ac, char**av, char**env)
{
	char	**cmd;

	if (ac == 1)
		return (0);
	cmd = parse(av + 1);
	if (!cmd)
		return (0);
	exec_cmds(cmd, env);
	restore(0);
	free(cmd);
}