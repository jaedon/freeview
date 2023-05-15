/*
	OCTO.ONE main file
	Just for Test - OCTO 2.0 One Process Model.
*/

#include	<hlib.h>

extern	int		hama_main(int argc, char *argv[]);
extern	int		hamalauncher_main(int argc, char *argv[]);
extern	int		dama_main(int argc, char *argv[]);
extern	int		cmddaemon_main(int argc, char *argv[]);
extern	int		imma_main(int argc, char *argv[]);
extern	int		umma_main(int argc, char *argv[]);
extern	int		tima_main(int argc, char *argv[]);
extern	int		thma_main(int argc, char *argv[]);
extern	int		pama_main(int argc, char *argv[]);
extern	int		sama_main(int argc, char *argv[]);
extern	int		nima_main(int argc, char *argv[]);
extern	int		obama_main(int argc, char *argv[]);
extern	int		homma_main(int argc, char *argv[]);

int	main(int argc, char *argv[])
{
	HxLOG_Warning("---- OCTO.ONE process is not launching....\n");

	HxLOG_Warning("start cmddaemon in OCTO.ONE\n");
	cmddaemon_main(argc, argv);

	HxLOG_Warning("start hamalauncher in OCTO.ONE.\n");
	hamalauncher_main(argc, argv);

	HxLOG_Warning("start hama in OCTO.ONE.\n");
	hama_main(argc, argv);

	HxLOG_Warning("start dama in OCTO.ONE.\n");
	dama_main(argc, argv);

	HxLOG_Warning("start obama in OCTO.ONE.\n");
	obama_main(argc, argv);

	HxLOG_Warning("start sama in OCTO.ONE.\n");
	sama_main(argc, argv);

	HxLOG_Warning("start pama in OCTO.ONE.\n");
	pama_main(argc, argv);

	HxLOG_Warning("start umma in OCTO.ONE.\n");
	umma_main(argc, argv);

	HxLOG_Warning("start thma in OCTO.ONE.\n");
	thma_main(argc, argv);

	HxLOG_Warning("start homma in OCTO.ONE.\n");
//	homma_main(argc, argv);

	HxLOG_Warning("start nima in OCTO.ONE.\n");
	nima_main(argc, argv);

	HxLOG_Warning("start imma in OCTO.ONE.\n");
	imma_main(argc, argv);

	HxLOG_Warning("start tima in OCTO.ONE.\n");
	tima_main(argc, argv);

	return 0;
}
