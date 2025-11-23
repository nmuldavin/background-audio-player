struct BackgroundAudioPlayer : Service::Switch
{ // First we create a derived class from the HomeSpan LightBulb Service

    DFRobot_DF1201S *player;
    SpanCharacteristic *power;

    // Next we define the constructor for DEV_LED.  Note that it takes one argument, ledPin,
    // which specifies the pin to which the LED is attached.

    BackgroundAudioPlayer(DFRobot_DF1201S *player) : Service::Switch()
    {

        power = new Characteristic::On(false);
        this->player = player;
    }

    boolean update()
    {
        if (this->power->getNewVal())
        {
            this->player->start();
        }
        else
        {
            this->player->pause();
        }

        return (true);
    }
};