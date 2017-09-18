import threading
import OpenValveScript
import sys
import time


try:
	processThread = threading.Thread(target=OpenValveScript.main, args=(3,))
	processThread.daemon = True
	processThread.start()
	#OpenValveScript.main()
	
	while True:
		print("huh")
		time.sleep(5)
		
	
except KeyboardInterrupt:
	sys.exit()


