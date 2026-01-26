#pragma once

#include "Steam/SteamClientPublic.h"
#include "editablepanel.h"
class KeyValues;

class CEconNotification
{
public:
	CEconNotification();
	virtual ~CEconNotification();

	void SetText( const char *pText ) { m_pText = pText; }
	void AddStringToken( const char* pToken, const wchar_t* pValue );

	void SetKeyValues( KeyValues *pKeyValues );
	KeyValues *GetKeyValues() const;

	const char* GetUnlocalizedText() { return m_pText; }
	const wchar_t *GetText();
	int GetID() const { return m_iID; };

	virtual void SetLifetime( float flSeconds ) { m_flExpireTime = flSeconds; };
	virtual float GetExpireTime() const;

	virtual float GetInGameLifeTime() const;
	
	void SetIsInUse( bool bInUse );
	bool GetIsInUse() const;

	void SetSteamID( const CSteamID &steamID );
	const CSteamID &GetSteamID() const;

	virtual bool BShowInGameElements() const { return true; }

	virtual void MarkForDeletion();

	enum EType {
		// Can only be deleted
		eType_Basic,
		// Can be accept or declined
		eType_AcceptDecline,
		// Can be triggered or deleted
		eType_Trigger,
		// Can only be triggered
		eType_MustTrigger,
	};

	virtual EType NotificationType();

	// Is this an important/high priority notification.  Triggers higher visibility etc..
	virtual bool BHighPriority();

	// eType_Trigger or eType_MustTrigger
	virtual void Trigger();

	// eType_AcceptDecline
	virtual void Accept();
	virtual void Decline();

	// eType_Basic or eType_Trigger
	virtual void Deleted();

	// All types, if expire time is set
	virtual void Expired();



	virtual void UpdateTick() { }

	virtual const char *GetUnlocalizedHelpText();

	virtual EditablePanel *CreateUIElement( bool bMainMenu ) const;

	void SetSoundFilename( const char *filename )
	{
		m_pSoundFilename = filename;
	}

	const char *GetSoundFilename() const
	{
		if ( m_pSoundFilename )
		{
			return m_pSoundFilename;
		}

		return "ui/notification_alert.wav";
	}

protected:
	const char *m_pText;
	const char *m_pSoundFilename;
	float m_flExpireTime;
	KeyValues *m_pKeyValues;
	wchar_t m_wszBuffer[1024];
	CSteamID m_steamID;

private:
	friend class CEconNotificationQueue;
	int m_iID;
	bool m_bInUse;
};
