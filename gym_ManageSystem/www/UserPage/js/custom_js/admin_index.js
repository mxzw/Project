//pie chart
$('#myStat3').circliful();
$('#myStat2').circliful();
//pie chart ends

//progress bar
$('.progress #pending.progress-bar').progressbar({
    transition_delay: 300
});
$('.progress #users.progress-bar').progressbar({
    transition_delay: 300
});
//progress bar ends

//user countup
var useOnComplete = false,
    useEasing = false,
    useGrouping = false,
    options = {
        useEasing: useEasing, // toggle easing
        useGrouping: useGrouping, // 1,000,000 vs 1000000
        separator: ',', // character to use as a separator
        decimal: '.' // character to use as a decimal
    }
var demo = new CountUp("myTargetElement4.2", 0, 1200, 0, 4, options);
demo.start();
var demo = new CountUp("myTargetElement4.1", 0, 900, 0, 4, options);
demo.start();
var demo = new CountUp("myTargetElement3.2", 0, 8527, 0, 4, options);
demo.start();
var demo = new CountUp("fb", 0, 21836, 0, 4, options);
demo.start();
var demo = new CountUp("tw", 0, 8613, 0, 4, options);
demo.start();
var demo = new CountUp("fifty", 0, 50, 0, 1, options);
demo.start();
var output = new CountUp("pending_amount", 0, 30, 0, 1, options);
output.start();
var my_posts = $("[rel=tooltip]");


var size = $(window).width();
for (i = 0; i < my_posts.length; i++) {
    the_post = $(my_posts[i]);

    if (the_post.hasClass('invert') && size >= 767) {
        the_post.tooltip({
            placement: 'left'
        });
        the_post.css("cursor", "pointer");
    } else {
        the_post.tooltip({
            placement: 'right'
        });
        the_post.css("cursor", "pointer");
    }
}
// user countup end



//growth analytics chart end


//user growth chart
var d11 = [
    ["Jan", 130],
    ["Mar", 110],
    ["May", 85],
    ["Jul", 50],
    ["Aug", 80],
    ["Sep", 145],
    ["Oct", 135],
    ["Nov", 82],
    ["Dec", 150]
];

var d12 = [
    ["Jan", 60],
    ["Mar", 45],
    ["May", 60],
    ["Jul", 33],
    ["Aug", 50],
    ["Sep", 50],
    ["Oct", 50],
    ["Nov", 30],
    ["Dec", 65]
];
$.plot("#bar-chart-stacked", [{
    data: d11,
    label: "Inquired Users",
    color: "#ff931d",
    yaxis: {
        tickLength: 0
    },
    xaxis: {
        tickLength: 0
    }
}, {
    data: d12,
    label: "Enrolled Users",
    color: "#33a4d8"
}], {
    series: {
        stack: !0,
        bars: {
            align: "center",
            lineWidth: 0,
            show: !0,
            barWidth: .5,
            fill: 1
        }

    },
    grid: {
        borderWidth: 0,
        hoverable: !0
    },
    legend: {
        container: '<div id="basicFlotLegend">  </div>',
        show: true
    },
    tooltip: !0,
    tooltipOpts: {
        content: "%x : %y",
        defaultTheme: true,
        tickLength: 0
    },
    xaxis: {
        tickColor: "#ddd",
        mode: "categories",
        tickLength: 0
    },
    shadowSize: 0
});
//user growth chart end


// Calendar 
/* initialize the calendar
 -----------------------------------------------------------------*/
//Date for the calendar events (dummy data)
var date = new Date();
var d = date.getDate(),
    m = date.getMonth(),
    y = date.getFullYear();
$('#calendar').fullCalendar({
    header: {
        left: 'title',
        right: 'prev,next'
    },
    //Random events

    editable: false,
    droppable: false,
    height: 450
});
// Calendar end


//c3 chart start

var data = [
    ['Previous Year', 'Present Year'],
    [20, 10],
    [19, 8],
    [33, 19],
    [27, 22],
    [12, 15],
    [19, 8],
    [31, 10],
    [39, 24],
    [37, 36],
    [30, 38],
    [20, 0],
    [28, 0]
];

//calculating current month
var currentMonth = new Date().getMonth();
console.log(currentMonth);

//c3 customisation

var chart = c3.generate({
    bindto: '#chart',
    data: {
        rows: data,
        type: 'spline'
    },
    color: {
        pattern: ['#33a4d8', '#65a800']
    },
    point: {
        select: {
            r: 4
        },
        r: 4
    },
    axis: {
        x: {
            tick: {
                format: function(d) {
                    return formatMonth(d);
                }
            }
        },
        y: {
            max: 50,
            min: 0,
            tick: {
                count: 7
            }
        }
    },
    legend: {
        show: true,
        position: 'bottom'
    },
    padding: {
        top: 10
    }
});

function formatMonth(d) {

    if (!d) {
        return "JAN";
    }
    var months = ["JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"];
    return month = months[d];
}
$('.right-side').resize(function() {
    chart.resize();
});

// c3 chart end
//calender

var eventDates = [1, 10, 12, 22],
    $picker = $('#custom-cells'),
    $content = $('#custom-cells-events'),
    sentences = [
        'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ita prorsus, inquam; Si enim ad populum me vocas, eum. Ita prorsus, inquam; Nonne igitur tibi videntur, inquit, mala? Hunc vos beatum; Idemne, quod iucunde? ',
        'Ratio quidem vestra sic cogit. Illi enim inter se dissentiunt. Tu vero, inquam, ducas licet, si sequetur; Non semper, inquam; ',
        'Duo Reges: constructio interrete. A mene tu? Ea possunt paria non esse. Est, ut dicis, inquam. Scaevolam M. Quid iudicant sensus? ',
        'Poterat autem inpune; Qui est in parvis malis. Prave, nequiter, turpiter cenabat; Ita credo. '
    ]
$picker.datepicker({
    language: 'en',
    onRenderCell: function(date, cellType) {
        var currentDate = date.getDate();
        if (cellType == 'day' && eventDates.indexOf(currentDate) != -1) {
            return {
                html: currentDate + '<span class="dp-note"></span>'
            }
        }
    },
    onSelect: function onSelect(fd, date) {
        var title = '',
            content = '';
        if (date && eventDates.indexOf(date.getDate()) != -1) {
            title = fd;
            content = sentences[Math.floor(Math.random() * eventDates.length)];
        }
        $('strong', $content).html(title);
        $('p', $content).html(content);
    }
});
var currentDate = new Date();
$picker.data('datepicker').selectDate(new Date(currentDate.getFullYear(), currentDate.getMonth(), 10));
