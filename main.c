#include "mlx.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <math.h>
#include "libft/libft.h"


#define HEIGHT 800
#define WIDTH 800
#define BUFFSIZE 5000
///usr/share/man/man3/mlx


typedef struct	s_cord 
{
	int 			x;
	int 			y;
	int				z;
	int				color;
}				t_cord;

typedef struct s_init
{
	void			*init;
	void			*img;
	void			*win;
	t_cord			t;
	int				att;
	int				r;
	int				g;
	int				b;
	unsigned char	*img_str;
	int				**map;
	t_cord			**tmp;
	t_cord			**v;
	t_cord			ini;
	int				mov;
	int				buff;
	float			vangl;
	float			hangl;
	int				projetction;
	char			*file;
}				t_init;

void	ft_light_pixel(t_init start, int x, int y)
{
	int i;
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return ;
	i = x * 4 + WIDTH * 4 * y;
	start.img_str[i] = start.b;
	start.img_str[i + 1] = start.g;
	start.img_str[i + 2] = start.r;
}

void	ft_vertical_line(unsigned char *img_str, int x, int y, int color)
{
	int i;
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return ;
	i = y * 4 + HEIGHT * 4 * x;
	img_str[i] = color;
	img_str[i + 1] = color;
	img_str[i + 2] = color;
}


void	slop_high(t_cord pi, t_cord pf, t_init start)
{
	int dx;
	int dy;
	int xi;
	int pxx;

	dx = abs(pf.x - pi.x);
	dy = pf.y - pi.y;
	xi = (pf.x - pi.x >= 0) ? 1 : -1;
	pxx = 2 * dx - dy;
	while (pi.y <= pf.y)
	{
		ft_light_pixel(start, pi.x, pi.y);
		if (pxx > 0)
		{
			pi.x += xi;
			pxx = pxx - (2 * dy);
		}
		pxx = pxx + (2 * dx);
		pi.y++;
	}
}

void	slop_low(t_cord pi, t_cord pf, t_init start)
{
	int dx;
	int dy;
	int yi;
	int pxx;

	dy = abs(pf.y - pi.y);
	dx = pf.x - pi.x;
	yi = (pf.y - pi.y >= 0) ? 1 : -1;
	pxx = 2 * dy - dx;
	while (pi.x <= pf.x)
	{
		ft_light_pixel(start, pi.x, pi.y);
		if (pxx >= 0)
		{
			pi.y += yi;
			pxx = pxx - (2 * dx);
		}
		pxx = pxx + (2 * dy);
		pi.x++;
	}
}

void	line_draw(t_cord pi, t_cord pf, t_init start)
{
	if (abs(pf.y - pi.y) < abs(pf.x - pi.x))
	{
		if (pi.x > pf.x)
			slop_low(pf, pi, start);
		else
			slop_low(pi, pf, start);
	}
	else
	{
		if (pi.y > pf.y)
			slop_high(pf, pi, start);
		else
			slop_high(pi, pf, start);
	}
}

int print_position(int button, int x, int y, t_init *start)
{
	/*static int ff;
	static int mx;
	static int my;
	t_cord v1, v2;

	if (ff == 0)
	{
		mx = x;
		my = y;
		ff++;
	}
	else if (ff == 1)
	{
		v1.x = mx;
		v1.y = my;
		v2.x = x;
		v2.y = y;	
		line_draw(v1, v2, start->img_str);
		ff = 0;
	}
	mlx_put_image_to_window(start->init, start->win, start->img, 0, 0);*/
	printf("this is the placement %d   %d    %d\n", button, x, y);
	return 1;
}

int str_isNum(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' && str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int first_line_length(char *file)
{
	int el;
	int i;

	i = 0;
	el = 0;
	while (file[i] != '\0' && file[i] != '\n')
	{
		if (file[i] != ' ')
		{
			while (file[i] != ' ')
			{
				if (file[i] == '\0' || file[i] == '\n')
					return (el + 1);
				i += 1;
			}	
			el++;
		}
		i += 1;
	}
	return (el);
}

int line_length_checker(char *file)
{
	int i;
	int el;
	int checker;

	i = 0;
	el = first_line_length(file);
	while (file[i] != '\0')
	{
		if (ft_isdigit(file[i]))
		{
			while (ft_isdigit(file[i]))
			{
				if (file[i] == '\0' || file[i] == '\n')
					break ;
				i++;
			}
			checker++;
		}
		if (file[i] == '\n' || file[i] == '\0')
		{
			if (checker < el)
			{
				ft_putstr("Found wrong line length. 1 Exiting.\n");
				return (0);
			}
			checker = 0;
		}
		i++;
	}
	return (1);
}

char	*file_checker(char *file)
{
	int i;
	int wrong;

	i = 0;
	wrong = 1;
	while (file[i])
	{
		if (file[i] == '\n' && file[i + 1] == '\n')
		{
			ft_putstr("Found wrong line length. Exiting.\n");
			wrong = 0;
			break ;
		}
		i++;
	}
	if (wrong == 1)
		wrong = line_length_checker(file);
	if (wrong == 1)
		return (file);
	else
		return (NULL);
}

int file_length(char **av)
{
	char	buf[BUFFSIZE + 1];
	int		ret;
	int		fd;
	int		i;

	i = 0;
	fd = open(av[1], O_RDONLY);
	if ((ret = read(fd, buf, BUFFSIZE)) == - 1)
	{
		ft_putstr("File was open but not read\n");
		exit(0);
	}
	else
		i += ret;
	while ((ret = read(fd, buf, BUFFSIZE)))
		i += ret;
	printf("this is length == %d\n", i);
	return (i);
}

// char *remove_spaces(char *file)
// {
// 	char *tmp;

// 	while (ft_strstr(file, "  "))
// 	{
// 		tmp = file;
// 		file = ft_find_replace(file, "  ", " ");
// 		free(tmp);
// 	}
// 	return (file);
// }

char *read_file(char **av)
{
	char	buf[BUFFSIZE + 1];
	int		ret;
	int		fd;
	char	*file;
	int		k;

	if ((fd = open(av[1], O_RDONLY)) == -1)
	{
		ft_putstr("Something wrong with the File\n");
		exit(0);
	}
	k = file_length(av);
	if (!(file = ft_strnew(k)))
	{
		ft_putstr("Not enaugh space in device\n");
		exit(0);
	}
	while ((ret = read(fd, buf, BUFFSIZE)))
		ft_strcat(file, buf);
	file[k] = '\0';
	return (file);
}

t_cord tab_length(char **tab)
{
	t_cord	t;
	int 	i;

	i = 0;
	t.x = 0;
	t.y = 0;
	while (tab[0][i])
	{
		if (tab[0][i] != ' ')
		{
			while (tab[0][i] != ' ' && tab[0][i] != '\0')
				i++;
			t.x++;
		}
		i++;
	}	
	while (tab[t.y])
		t.y++;
	return (t);
}

void	tab_free(char **tab)
{
	int i;

	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
	tab = NULL;
}

int		find_the_coma(char *str)
{
	int i;

	i = 0;
	while (str[i] && str[i] != ',')
		i++;
	return (i);
}

int		index_of(char *str, char c)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

int		color_converter(char *hex)
{
	char	str[16] = "0123456789ABCDEF";
	int		tmp;
	int		i;
	int		j;

	i = 0;
	j = 0;
	tmp = 0;
	while (hex[i])
		i++;
	while (i-- > 2)
	{
		tmp += (index_of(str, hex[i]) * pow(16, j));
		j++;
	}
	return (tmp - 1);
}

void		map_alloc_2(char **tab, int i, t_cord **map, t_init *start)
{
	int j;
	char **str;
	int o;
	int k;

	str = ft_strsplit(tab[i], ' ');
	j = 0;
	if (i == 0)
		o = 0;
	k = 0;
	while (j < start->t.x)
	{
		if (i != 0 || j != 0)
		{
			map[i][j].x = k - (start->t.x * start->buff / 2);
            map[i][j].y = o - (start->t.y * start->buff / 2);
		}
		if (ft_strstr(str[j], ","))
		{
			map[i][j].color = color_converter(str[j] + find_the_coma(str[j]) + 1);
			str[j][find_the_coma(str[j])] = '\0';
			map[i][j].z = ft_atoi(str[j]);
		}
		else
		{
			map[i][j].z = ft_atoi(str[j]);
			map[i][j].color = -1;
		}
		k += start->buff;
		j++;
	}
	o += start->buff;
	tab_free(str);
}

t_cord    **map_alloc(char **av, t_init *start)
{	
	char 	**tab;
	char	*file;
	int 	i;
	t_cord **map;

	i = 0;
	tab = ft_strsplit(start->file, '\n');
	//free(start->file);
	start->t = tab_length(tab);
	//printf("this is x == %d this us y == %d\n", start->t.x, start->t.y);
	map = (t_cord **)malloc(sizeof(t_cord*) * start->t.y);
    while(i < start->t.y)
        map[i++] = (t_cord *)malloc(sizeof(t_cord)* start->t.x);
	start->buff = (WIDTH / (start->t.x * 1.5));
	map[0][0].x = -(start->t.x * start->buff / 2);
    map[0][0].y = -(start->t.y * start->buff / 2);
	i = 0;
	while (i < start->t.y)
		map_alloc_2(tab, i++, map, start);
	tab_free(tab);
	return (map);
}

static void rotation_vertical(int *x, int *y, int *z, t_init start)
{
    int previous_x;
	int previous_z;

    previous_x = *x;
	previous_z = *z;
    *x = previous_x * cos(start.vangl) + previous_z * sin(start.vangl);
    *z = previous_x * sin(start.vangl) + previous_z * cos(start.vangl);
}

static void rotation_horizontal(int *x, int *y, int *z, t_init start)
{
    int previous_y;
	int previous_z;

    previous_y = *y;
	previous_z = *z;
    *y = (previous_y * cos(start.hangl) + previous_z * sin(start.hangl));
    *z = (previous_y * sin(start.hangl) + previous_z * cos(start.hangl));
}

static void iso(int *x, int *y, int z, int mov)
{
    int previous_x;
    int previous_y;

    previous_x = *x;
    previous_y = *y;
    *x = (previous_x - previous_y) * cos(0.523599) + mov;
    *y = -z + (previous_x + previous_y) * sin(0.523599);
}

void	projec_iso(t_cord **map, t_init *start)
{
	int i;
	int j;

	i = 0;
	while(i < start->t.y)
	{
		j = 0;
		while(j < start->t.x)
		{
			iso(&map[i][j].x, &map[i][j].y, start->att * map[i][j].z, start->mov);
			rotation_horizontal(&map[i][j].x, &map[i][j].y, &map[i][j].z, *start);
			rotation_vertical(&map[i][j].x, &map[i][j].y, &map[i][j].z, *start);
			j++;
		}
		i++;
	}
}

void	projec_parallel(t_cord **map, t_init *start)
{
	int i;
	int j;

	start->ini.x = 0;
	start->ini.y = 0;
	i = 0;
	while(i < start->t.y)
	{
		j = 0;
		while(j < start->t.x)
		{
			map[i][j].z = start->att * map[i][j].z;
			rotation_horizontal(&map[i][j].x, &map[i][j].y, &map[i][j].z, *start);
			rotation_vertical(&map[i][j].x, &map[i][j].y, &map[i][j].z, *start);
			j++;
		}
		i++;
	}
}

void drow(t_init start, t_cord **map)
{
    
    int i;
    int j;
    t_cord s;
    t_cord f;
    
    i = 0;
    while (i < start.t.y)
    {
        j = 0;
        while (j < start.t.x)
        {
            s = map[i][j];
			s.x = s.x +400;
			s.y = s.y +400;
            if (j + 1 < start.t.x)
            {
                f = map[i][j + 1];
				f.x = f.x +400;
				f.y = f.y +400;
                line_draw(s, f, start);
            }
            if (i + 1 < start.t.y)
            {
                f = map[i + 1][j];
				f.x = f.x +400;
				f.y = f.y +400;
                line_draw(s, f, start);
            }
            j++;
        }
        i++;
    }
}

t_cord    **make_clone(t_init t)
{
	int i;
	int j;
	t_cord **map;

	i = 0;
	map = (t_cord **)malloc(sizeof(t_cord*) * t.t.y);
    while(i < t.t.y)
        map[i++] = (t_cord *)malloc(sizeof(t_cord)* t.t.x);
    map[0][0].x = t.ini.x;
    map[0][0].y = t.ini.y;
	i = 0;
	while (i < t.t.y)
	{
		j = 0;
		while (j < t.t.x)
		{
			map[i][j].x = t.v[i][j].x;
			map[i][j].y = t.v[i][j].y;
			map[i][j].z = t.v[i][j].z;
			j++;
		}
		i++;
	}
	return (map);
}

void	color_function(t_init *start, int color)
{
	if (color == 0)
	{
		if ((start->r + 10) <= 255)
			start->r += 10;
		if ((start->g - 10) >= 0)
			start->g -= 10;
		if ((start->b - 10) >= 0)
			start->b -= 10;
	}
	else if (color == 1)
	{
		if ((start->g + 10) <= 255)
			start->g += 10;
		if ((start->b - 10) >= 0)
			start->b -= 10;
		if ((start->r - 10) >= 0)
			start->r -= 10;
	}
	else if (color == 2)
	{
		if ((start->b + 10) <= 255)
			start->b += 10;
		if ((start->g - 10) >= 0)
			start->g -= 10;
		if ((start->r - 10) >= 0)
			start->r -= 10;
	}
}

void	map_free(t_cord **map, t_init start)
{
	int i;

	i = 0;
	while (i < start.t.y)
		free(map[i++]);
	free(map);
	map = NULL;
}

int key_press(int button, t_init *start)
{
	// int b_p;
	// int s_l;
	// int endian;

	mlx_clear_window(start->init, start->win);
	mlx_destroy_image(start->init, start->img);
	start->img = mlx_new_image(start->init, HEIGHT, WIDTH);
	//start->img_str = (unsigned char *)mlx_get_data_addr(start->img, &b_p, &s_l, &endian);
	if (button == 53)
	{
		map_free(start->v, *start);
		map_free(start->tmp, *start);
		exit(0);
	}
	if (button == 126)
		start->att += 1;
	else if (button == 125)
	{
		// if (start->att == 1)
		// 	start->att -= 1;
		start->att -= 1;
	}	
	else if (button == 124)
		start->mov += 10;
	else if (button == 123)
		start->mov -= 10;
	else if (button == 91)
		start->hangl -= 0.1;
	else if (button == 84)
		start->hangl += 0.1;
	else if (button == 86)
		start->vangl -= 0.1;
	else if (button == 88)
		start->vangl += 0.1;
	else if (button == 15)
		color_function(start, 0);
	else if (button == 5)
		color_function(start, 1);
	else if (button == 11)
		color_function(start, 2);
	else if (button == 34)
	{
		start->vangl = 0;
		start->hangl = 0;
		start->projetction = 1;
	}
	else if (button == 35)
	{
		start->vangl = 0;
		start->hangl = 0;
		start->projetction = 2;
	}
	start->tmp = make_clone(*start);
	if (start->projetction == 1)
		projec_iso(start->tmp, start);
	if (start->projetction == 2)
		projec_parallel(start->tmp, start);
	drow(*start, start->tmp);
	mlx_put_image_to_window(start->init, start->win, start->img, 0, 0);
	printf("this is the placement %d  \n", button);
	return 1;
}

void	init_prog(t_init *start, char **av)
{
	int b_p;
	int s_l;
	int endian;

	start->init = mlx_init();
	start->win = mlx_new_window(start->init, HEIGHT, WIDTH, "fdf");
	start->img = mlx_new_image(start->init, HEIGHT, WIDTH);
	start->img_str = (unsigned char *)mlx_get_data_addr(start->img, &b_p, &s_l, &endian);
	start->ini.x = 0;
	start->ini.y = 0;
	start->hangl = 0;
	start->hangl = 0;
	start->r = 255;
	start->g = 0;
	start->b = 255;
	start->att = 1;
	start->mov = 0;
	start->v = map_alloc(av, start);
	start->tmp = make_clone(*start);
	start->projetction = 1;
}

int main(int ac, char **av)
{
	t_init start;
	int i = 0;
	int j = 0;

	if (ac == 2)
	{
		if (!(start.file = file_checker(read_file(av))))
			return (0);
		init_prog(&start, av);
		projec_iso(start.tmp, &start);
		drow(start, start.tmp);
		mlx_put_image_to_window(start.init, start.win, start.img, 0, 0);
		mlx_hook(start.win, 2, 0, key_press, &start);
		mlx_loop(start.init);
	}
	else
		ft_putstr("Usage : ./fdf <filename>\n");
}
