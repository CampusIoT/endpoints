/**
* Simple program for Sagemcom Siconia : send sensors values periodically and on press released
* @Link https://github.com/CampusIoT/tutorial/tree/master/siconia
* @Link http://iot.siconia.sagemcom.com:8080/loradevice-doc/
**/

/* interval used to send sensors values */
const INTERVAL = 20*60*1000; // 20 minutes
const DUTYCYCLE = true;

const DELAY = 6000; // in ms

const REPORT_PORT=1;
const BUTTON_PORT=7;

function toUint8(v) {
	var buffer = new ArrayBuffer(1);
	var i16 = new Uint8Array(buffer);
	i16[0] = v;
	return buffer;
}

function toInt16(v) {
	var buffer = new ArrayBuffer(2);
	var i16 = new Int16Array(buffer);
	i16[0] = v;
	return buffer;
}

function toUint16(v) {
	var buffer = new ArrayBuffer(2);
	var i16 = new Uint16Array(buffer);
	i16[0] = v;
	return buffer;
}

function sendTHPB(event) {
  if(!started) {
    red.blink(4);
    return;
  }
	// check timeOff
	//if(L.getTimeOff() > 0) return;

	var payload = new ArrayBuffer(5);
	var i8=new Int8Array(payload);
	var data=new Int16Array(payload);

	// T in C° size (8 bits)
	T.init();
	wait(50);
	i8[0]=T.read();
	T.deinit();

	// Hu in % size (8 bits)
	Hu.init();
	wait(50);
	i8[1]=Hu.read();
	Hu.deinit();

	// Pressure  in hPa size (16 bits)
	Pr.init();
	wait(50);
	data[1]=Pr.read()/100;
	Pr.deinit();

	// battery level in % (8 bits)
	i8[4]=Board.battery().percent;

	L.send(event,payload);
}

function sendReport() {
	sendTHPB(REPORT_PORT);
}

function onReleaseEvent(delay) {
  if (delay > DELAY) {
    if (started == false) {
			startup();
		} else {
			L.airplane(true);
      red.blink(4);
			started = false;
			//hwreset();
		}
  } else {
		sendTHPB(BUTTON_PORT);
	}
}

var intervalID;

var started = false;

function stop() {
    L.dutycycle(true);
    L.airplane(true);
		started = false;
}

function onJoin() {
    green.blink(2);
    orange.blink(2);
    green.blink(2);
    //print("node has joined devAddr="+L.devAddr());

		M.init();
		intervalID = setInterval(sendReport, INTERVAL);
}

function onTx(tx_status) {
    orange.blink(1);
		//print("tx tx_status="+tx_status);
}

function onEndTx(tx_status) {
    green.blink(1);
}

function onRx(parameters) {
    green.blink(2);
    // TODO should process the downlink
}

// Called on button release
B.on('release', onReleaseEvent);

// LoRaWAN Events section
// The ‘join’ event is called when the device join the network.
L.on('join', onJoin);

// The ‘tx’ event is called when a frame has been sent.
L.on('tx', onTx);
// The ‘endtx’ event is called once the complete transmission procedure (including an eventual reception) is finished.
// L.on('‘endtx’', onEndTx);

// The ‘rx’ event is called when a frame has been received.
L.on('rx', onRx);

// Passed parameters : 'size' -> Size of the MAC reply in bytes
//L.on('mac', function() { ... });

/* Startup */
function startup() {
    red.blink(2);
    orange.blink(2);
    red.blink(2);

    L.airplane(false);
    L.dutycycle(DUTYCYCLE);
		started = true;
		L.join();
}

Board.on('init', startup);
