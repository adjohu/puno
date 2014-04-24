dnl $Id$
dnl config.m4 for extension puno

PHP_ARG_WITH(puno, for OpenOffice.org support, [  --with-puno[=DIR]	  include OpenOffice.org UNO support. DIR is the OpenOffice.org SDK directory (Requires OpenOffice.org 3.x and OpenOffice.org SDK 3.x ). ])

if test "$PHP_PUNO" != "no"; then

  if test "$PHP_PUNO" = "yes"; then
 	PHP_PUNO=/opt/openoffice.org/basis3.2/sdk/
	PHP_SUBST(PHP_PUNO)	
  fi

  
  AC_CACHE_CHECK([for OpenOffice.org URE types.rdb ], puno_cv_types_rdb_path,
  [
  	puno_cv_types_rdb_path="no"
  	if test -f "$OO_SDK_URE_HOME/share/misc/types.rdb"; then  	
  		puno_cv_types_rdb_path="$OO_SDK_URE_HOME/share/misc/types.rdb"
  	fi
  	if test -f "$OO_URE_TYPES_RDB_PATH"; then  	
  		puno_cv_types_rdb_path="$OO_URE_TYPES_RDB_PATH"
  	fi
  ])
  
  if test "$puno_cv_types_rdb_path" = "no"; then
    AC_MSG_ERROR([ File types.rdb could not be found. Check your OO-SDK environment configuration. Alternatively, you can set the shell variable \$OO_URE_TYPES_RDB_PATH with types.rdb fullpath. ])
  fi

  PHP_PUNO_URE_TYPES_RDB_PATH=$puno_cv_types_rdb_path
  PHP_SUBST(PHP_PUNO_URE_TYPES_RDB_PATH)
	
  
  AC_CACHE_CHECK([for OpenOffice.org PROGRAM offapi.rdb ], puno_cv_offapi_rdb_path,
  [
  	puno_cv_offapi_rdb_path="no"
  	if test -f "$OFFICE_PROGRAM_PATH/offapi.rdb"; then
  		puno_cv_offapi_rdb_path="$OFFICE_PROGRAM_PATH/offapi.rdb"
  	fi
  	
  	if test -f "$OFFICE_BASE_PROGRAM_PATH/offapi.rdb"; then
  		puno_cv_offapi_rdb_path="$OFFICE_BASE_PROGRAM_PATH/offapi.rdb"
  	fi
  	
  	if test -f "$OO_PROGRAM_OFFAPI_RDB_PATH"; then  	
  		puno_cv_offapi_rdb_path="$OO_PROGRAM_OFFAPI_RDB_PATH"
  	fi
  ])
	
  if test "$puno_cv_offapi_rdb_path" = "no"; then
    AC_MSG_ERROR([ File offapi.rdb could not be found. Check your OO-SDK environment configuration. Alternatively, you can set the shell variable \$OO_PROGRAM_OFFAPI_RDB_PATH with offapi.rdb fullpath. ])
  fi
  
  PHP_PUNO_PROGRAM_OFFAPI_RDB_PATH=$puno_cv_offapi_rdb_path
  PHP_SUBST(PHP_PUNO_PROGRAM_OFFAPI_RDB_PATH)  
	  

  PHP_REQUIRE_CXX()

  PRJ=$PHP_PUNO
  PHP_SUBST(PRJ)

  OUT="\$(srcdir)/.libs/puno_automatic_generated"
  PHP_SUBST(OUT)

  PHP_ADD_LIBRARY(stdc++, 1, PUNO_SHARED_LIBADD)

    	

  EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS \$(CC_FLAGS) \$(CC_INCLUDES) -I\$(OUT_COMP_INC) \$(CC_DEFINES)"
  PHP_SUBST(EXTRA_CXXFLAGS)	   

  EXTRA_LIBS="$EXTRA_LIBS \$(LINK_LIBS)"
  PHP_SUBST(EXTRA_LIBS)	   
 
  EXTRA_LDFLAGS="$EXTRA_LDFLAGS  \$(LINK_LIBS) \$(CPPUHELPERLIB) \$(CPPULIB) \$(SALHELPERLIB) \$(SALLIB) \$(STLPORTLIB)"
  PHP_SUBST(EXTRA_LDFLAGS)	   

	PHP_MODULES="cppumaker $PHP_MODULES"
	PHP_SUBST(PHP_MODULES)	
	      
  AC_DEFINE(HAVE_PUNOLIB,1,[ ])

  PHP_SUBST(PUNO_SHARED_LIBADD)

 	
  PHP_ADD_MAKEFILE_FRAGMENT(Makefile.cppumaker)
  PHP_NEW_EXTENSION( puno, puno.cpp, $ext_shared,,,cxx)
fi
