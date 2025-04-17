#pragma once

#ifdef BENGIN

	#ifdef BENGIN_BUILD_DLL

		#define	BENGIN_API __declspec (dllexport)

	#else	

		#define	BENGIN_API __declspec (dllimport)

	#endif // BENGIN_BUILD_DLL

#else

	#error BenGin not defined

#endif // BENGIN
