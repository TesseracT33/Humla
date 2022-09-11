module Core;

import Emulator;
import Input;

void Core::SetupCommunicationWithFrontend()
{
	Input::SetCoreActionNames(this->GetActionNames());
}