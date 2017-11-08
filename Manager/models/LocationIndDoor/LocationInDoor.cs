namespace Manager.Models
{
    public class LocationInDoor : Configuration
    {
        public bool IsEnable { get; set; }
        public double Interval { set; get; }
        public int iBeaconNumber { set; get; }
        public bool IsEmergency { set; get; }

        public LocationInDoor()
        {
            IsEnable = false;
            Interval = 30;
            iBeaconNumber = 5;
            IsEmergency = false;
        }
    }
}