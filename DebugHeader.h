#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace  DebugHeader 
{
	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message,
	bool bShowMsgAsWarning = true)
	{
		if(bShowMsgAsWarning)
		{
			FText MsgTitle = FText::FromString(TEXT("Warning"));

			//return FMessageDialog::Open(MsgType,FText::FromString(Message));
			return FMessageDialog::Open(MsgType, FText::FromString(Message), &MsgTitle);
			
		}
		else
		{
			return FMessageDialog::Open(MsgType, FText::FromString(Message));
		}
	}

	static void ShowNotifyInfo(const FString& Message)
	{
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.f;

		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}
}
