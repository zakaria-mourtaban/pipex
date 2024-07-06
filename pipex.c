/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmourtab <zakariamourtaban@gmail.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 10:27:36 by zmourtab          #+#    #+#             */
/*   Updated: 2024/07/02 12:35:39 by zmourtab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"

void	exec_command(char *cmd, char **env)
{
	char	**s_cmd;
	char	*path;

	s_cmd = ft_split(cmd, ' ');
	path = get_path(s_cmd[0], env);
	if (execve(path, s_cmd, env) == -1)
	{
		ft_putstr_fd("pipex: command not found: ", 2);
		ft_putendl_fd(s_cmd[0], 2);
		ft_free_tab(s_cmd);
		exit(127);
	}
}

void	first_child(char **argv, char **env, int pipefd[2])
{
	int	infile;

	infile = open_file(argv[1], 0);
	if (infile == -1)
		error_handling("Failed to open infile");
	if (dup2(infile, STDIN_FILENO) == -1)
		error_handling("Dup2 failed for infile");
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		error_handling("Dup2 failed for pipefd[1]");
	close(pipefd[0]);
	close(pipefd[1]);
	close(infile);
	exec_command(argv[2], env);
}

void	second_child(char **argv, char **env, int pipefd[2])
{
	int	outfile;

	outfile = open_file(argv[4], 1);
	if (outfile == -1)
		error_handling("Failed to open outfile");
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		error_handling("Dup2 failed for pipefd[0]");
	if (dup2(outfile, STDOUT_FILENO) == -1)
		error_handling("Dup2 failed for outfile");
	close(pipefd[0]);
	close(pipefd[1]);
	close(outfile);
	exec_command(argv[3], env);
}

int	pipex(char *argv[], char *env[])
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status;

	if (pipe(pipefd) == -1)
		error_handling("pipe failed");
	pid1 = fork();
	if (pid1 == -1)
		error_handling("Fork failed");
	if (pid1 == 0)
		first_child(argv, env, pipefd);
	pid2 = fork();
	if (pid2 == -1)
		error_handling("Fork failed");
	if (pid2 == 0)
		second_child(argv, env, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	return (status);
}

int	main(int argc, char *argv[], char *env[])
{
	if (argc != 5)
	{
		ft_putstr_fd("Usage: ./pipex file1 cmd1 cmd2 file2\n", 2);
		return (1);
	}
	return (pipex(argv, env));
}
