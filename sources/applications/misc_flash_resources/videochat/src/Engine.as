package
{
	import com.rtmpd.generics.Connection;

	public class Engine extends Connection
	{
		public function Engine()
		{
		}
		public function onBWDone(info:Object):void{
			trace("info=" + info.toString());
		}
	}
}