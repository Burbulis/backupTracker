#ifndef  __SQL_COPY__
#define	__SQL_COPY__

#include "sqlcmdType.hpp"
#include "insert.hpp"
#include "selectAndJoin.hpp"
#include "sqlGenerator.hpp"
namespace SQLCMD
{
	std::string
	SqlCopy(std::string sqlTableFrom, std::string sqlTableTo, SQLCMD::selector_type fildsDesc)
	{
		SQLCMD::Generator <  SQLCMD::selector_type, SQLCMD::Select< > > _select(sqlTableFrom, fildsDesc);
		SQLCMD::Generator <  SQLCMD::selector_type, SQLCMD::halfInsert > insertInto(sqlTableTo, fildsDesc);
		return (insertInto() + " " + _select());
	}

}
#endif // ! 
