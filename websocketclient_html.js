import {
  ComplexArray
} from './node_modules/jsfft/dist/fft.js'

import {
  Chart,
  ArcElement,
  LineElement,
  BarElement,
  PointElement,
  BarController,
  BubbleController,
  DoughnutController,
  LineController,
  PieController,
  PolarAreaController,
  RadarController,
  ScatterController,
  CategoryScale,
  LinearScale,
  LogarithmicScale,
  RadialLinearScale,
  TimeScale,
  TimeSeriesScale,
  Decimation,
  Filler,
  Legend,
  Title,
  Tooltip
} from './node_modules/chart.js/dist/chart.js';

Chart.register(
  ArcElement,
  LineElement,
  BarElement,
  PointElement,
  BarController,
  BubbleController,
  DoughnutController,
  LineController,
  PieController,
  PolarAreaController,
  RadarController,
  ScatterController,
  CategoryScale,
  LinearScale,
  LogarithmicScale,
  RadialLinearScale,
  TimeScale,
  TimeSeriesScale,
  Decimation,
  Filler,
  Legend,
  Title,
  Tooltip
);
/* Chart.js stuff */
function updateChart() {
  RCdata.labels = Array.from(Array(x_data.length).keys())
  rawChart.update();
}
document.addEventListener("DOMContentLoaded", function() {
  /* detect if page is being served from ESP or from local*/
  if (location.protocol == "file:") {
    //is local file on PC, can't autofill ip
    console.log("Local page")
  } else {
    //is probably hosted from esp32
    let ip = location.hostname
    console.log(`Remote page from IP: ${ip}`)
    document.getElementById("ip").value = ip
  }
  /* RAW DATA CHART */
  const RCctx = document.getElementById('rawCanv');
  var x_data = new Array(1000) //raw x acceleration
  var y_data = new Array(1000) //raw y acceleration
  var z_data = new Array(1000) //raw z acceleration
  var v_data = new Array(1000) //resulting vector from components x, y, z
  window.x_data = x_data
  window.y_data = y_data
  window.z_data = z_data
  window.v_data = v_data
  var RCdata = {
    labels: [],
    datasets: [{
        label: 'X',
        data: x_data,
        fill: false,
        borderColor: 'rgb(255, 0, 0)',
        tension: 0.8
      },
      {
        label: 'Y',
        data: y_data,
        fill: false,
        borderColor: 'rgb(0, 255, 0)',
        tension: 0.8
      },
      {
        label: 'Z',
        data: z_data,
        fill: false,
        borderColor: 'rgb(0, 0, 255)',
        tension: 0.8
      }
    ]
  };
  window.RCdata = RCdata
  const RCconfig = {
    type: 'line',
    data: RCdata,
    options: {
      maintainAspectRatio: false,
      radius: 0,
      borderWidth: 1
      /*parsing: !1,
      normalized: !0,
      animation: !1,
      plugins: {
          legend: !1,
          decimation: {
            enabled: !0
        }
      }*/
    }
  };
  window.RCconfig = RCconfig
  const rawChart = new Chart(RCctx, RCconfig)
  rawChart.options.animation = false;
  window.rawChart = rawChart
  /* FFT CHART */
  const FCctx = document.getElementById('fftCanv')
  var fft_data;
  window.fft_data = fft_data
  var FCdata = {
    labels: [],
    datasets: [{
      label: "Frequencies",
      data: fft_data,
      fill: false,
      borderColor: '#000000',
      tension: 0.8,
    }]
  };
  window.FCdata = FCdata
  const FCconfig = {
    type: 'line',
    data: FCdata,
    options: {
      maintainAspectRatio: false,
      radius: 0,
      borderWidth: 1
    }
  };
  window.RCconfig = RCconfig
  const fftChart = new Chart(FCctx, FCconfig)
  fftChart.options.animation = false;
  window.fftChart = fftChart
  /* File export setup */
  let fileData = []
  window.fileData = fileData
  /* "exports" */
  window.connect = connect
  let counter = 0
  window.counter = counter
  window.download_CSV = download_CSV
  window.disconnect = disconnect
  window.socket = false
  window.save = false
})
/* Websocket stuff */
// Create WebSocket connection.

function connect() {
  const socket = new WebSocket('ws://' + document.getElementById("ip").value + ':80/ws');

  // Connection opened
  socket.addEventListener('open', function(event) {
    console.log("connected");
  });

  // Listen for messages
  socket.addEventListener('message', function(event) {
    graphRaw(event.data)
  });

  socket.addEventListener('close', function(event) {
    console.log("disconnect occured")
  })
  window.socket  = socket
}
function disconnect() {
  if (socket){
    socket.close()
  } else {
    console.log("nothing to close!")
  }
}
/* signal processing stuff */
function graphRaw(data) {
  let pos = data.split(" ").filter(Number);
  x_data.shift()
  x_data.push(pos[0])
  y_data.shift()
  y_data.push(pos[1])
  z_data.shift()
  z_data.push(pos[2])
  v_data.shift()
  v_data.push(Math.sqrt(pos[0]**2+pos[1]**2+pos[2]**2))
  counter +=1
  if (counter%10==0) {
    updateChart()
  }
  if (counter%100==0) {
    let processed = fftData(v_data)
    FCdata.labels = processed.imag//Array.from(Array(processed.real.length).keys())
    processed.real[0] = 0
    FCdata.datasets[0].data = processed.real
    fftChart.update()
  }
  if (counter%1000 == 0 && save == true) {//entire array filled once! save this to file thingy
    for (let i=0;i<1000;i++){
      fileData.push([x_data[i], y_data[i], z_data[i]])
    }
    counter = 0
  }
}

function fftData(rawdata) {
  const data = new ComplexArray(512).map((value, i, n) => {
    value.real = rawdata[999-i]
  })
  data.FFT()
  return data
}

/* file export stuff */
function download_CSV(){
  var csv = 'X,Y,Z\n'
  fileData.forEach(row => {
    csv+=row.join(",")
    csv+="\n"
  })
  document.write(csv)
  var hiddenElement = document.createElement('a');
  hiddenElement.href = 'data:text/csv;charset=utf-8'+encodeURI(csv);
  hiddenElement.target = "_blank"
  hiddenElement.download = "Raw Sensor Data.csv"
  hiddenElement.click()
}
