#pragma once

#include "Param/GSParam.h"

class GSResult
{
public:
	GSResult(void);
	~GSResult(void);
	
	typedef std::vector< std::wstring >	QUERY_RESULT;

	QUERY_RESULT	m_Result;

	u32				m_uiResultPos;
	bool			m_bResult;					
	std::wstring	m_Query;

	GSResult&		operator >>		( s64&			data	);
	GSResult&		operator >>		( std::wstring&	data	);
	GSResult&		operator >>		( bool&			data	);

	const std::wstring&	GetValueAndNextPos();
	bool				IsGetDataEmpty() const;

	template < class _T >
	GSResult& operator >>	( _T& data ) 
	{
		const std::wstring&	strResult	= db.GetValueAndNextPos();

		if( strResult.length() == 0 )
		{
			data	= static_cast<_T>(0);
		}
		else
		{
			WCHAR *	code = NULL;
			int		radix = 10;
			data	= static_cast<_T>( wcstoul ( strResult.c_str(), &code, radix ) );
		}

		return db;
	}
};

