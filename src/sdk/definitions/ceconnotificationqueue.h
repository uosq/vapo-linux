#pragma once

#include "ceconnotification.h"
#include "utlvector.h"

class CEconNotificationQueue
{
public:
	CEconNotificationQueue();
	~CEconNotificationQueue();

	int AddNotification( CEconNotification *pNotification );
	void RemoveAllNotifications();
	void RemoveNotification( int iID );
	void RemoveNotification( CEconNotification *pNotification );
	void RemoveNotifications( void* func );
	int CountNotifications( void* func );
	void VisitNotifications( void** visitor );
	CEconNotification *GetNotification( int iID );
	CEconNotification *GetNotificationByIndex( int idx );
	void Update();
	bool HasItems() { return m_vecNotifications.Count() != 0; }
	const CUtlVector< CEconNotification *> &GetItems() { return m_vecNotifications; }

private:
	int m_iIDGenerator;
	CUtlVector< CEconNotification *> m_vecNotifications;
};