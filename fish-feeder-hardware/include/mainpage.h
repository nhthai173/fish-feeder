const char *MAINPAGE = R"html(
<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous"><link href="https://cdn.jsdelivr.net/gh/hung1001/font-awesome-pro@4cac1a6/css/all.css" rel="stylesheet" type="text/css"><title>Fish Feeder</title><style>body{background:rgba(0 0 0 / 3%)}.main-view{margin-top:50px;margin-bottom:50px;width:100%;max-width:600px}.loading{position:fixed;top:0;left:0;width:100%;height:100%;display:flex;justify-content:center;align-items:center;user-select:none;z-index:1000}section{position:relative}.back-to-home{position:absolute;top:0;left:20px;cursor:pointer}.back-to-home::before{font-family:'Font Awesome 5 Pro';content:"\f177";font-weight:100;font-size:2.2em;color:var(--bs-body-color)}.loading .message{position:absolute;left:50%;bottom:50px;transform:translateX(-50%);padding:20px;border-radius:200px;font-size:1.2em;color:var(--bs-white);background:rgba(0 0 0 / 60%)}.card{margin-bottom:40px;border-width:0;border-radius:12px;box-shadow:0 0 20px 0 rgba(0,0,0,.05);padding:15px;display:flex;flex-direction:row}.card-logo-container{width:100px;height:100%;display:flex;justify-content:center;align-items:center;padding:20px 0 20px 0}.card-logo{width:65px;height:65px;border-radius:50%;display:flex;justify-content:center;align-items:center;color:var(--bs-white);background:var(--bs-primary)}.card-content{display:flex;align-items:center;padding-top:10px;margin-left:15px}.form-check-lg{width:60px!important;height:30px}.modal-header{border-width:0;padding:20px}.disabled{pointer-events:none!important;opacity:.5}.btn-p{width:100%;border-radius:200px}.btn-p-sm{max-width:150px}.btn-add{position:fixed;bottom:60px;right:60px;width:60px;height:60px;border-radius:50%;background:var(--bs-primary);box-shadow:0 0 20px 0 rgba(0,0,0,.1);cursor:pointer}.btn-add::before{font-family:'Font Awesome 5 Pro';content:"\f067";font-weight:100;font-size:2.2em;color:var(--bs-white);position:absolute;top:50%;left:50%;transform:translate(-50%,-50%)}.expand-icon::before{font-family:'Font Awesome 5 Pro';content:"\f054";font-weight:450;font-size:1.1em;color:rgba(var(--bs-secondary-rgb),.5)}.timeline{position:relative;width:100%;height:40px;padding-top:20px;display:flex}.timeline .time::before{content:'';position:absolute;top:10px;left:95px;width:5px;height:5px;border-radius:50%;background-color:rgba(0 0 0 / 20%)}.timeline:not(:last-child) .time::after{content:'';position:absolute;top:10px;left:96.5px;width:2px;height:calc(100% + 16px);background-color:rgba(0 0 0 / 20%)}.timeline .time{position:absolute;top:0}.timeline .activity{position:absolute;top:0;left:110px}@media screen and (max-width:500px){.loading .message{padding:10px;font-size:.9em;bottom:30px}.card{padding:0}.btn-add{bottom:25px;right:25px}.back-to-home{left:15px;top:-6px;font-size:.8em}}</style></head><body><div class="loading d-none"><div class="message">Đã đặt lượng thức ăn: 2g</div></div><div class="container-fluid"><div class="row justify-content-center"><div class="col main-view placeholder-glow"><section class="fade show" id="home"><div class="mb-3" style="width:200px;height:200px"><img src="https://i.imgur.com/0T2f9R1.png" class="img-fluid"></div><h2 class="mb-5 mb-md-4 fs-1 fs-md-2">Máy cho cá ăn</h2><div class="card d-flex" id="feed-card"><div class="card-logo-container"><div class="card-logo"><i class="fa-2x fas fa-braille"></i></div></div><div class="card-content w-100"><div class="w-100 ms-3 ms-md-1"><h4 class="mb-1" id="feed-text">Cho ăn</h4><span class="text-muted">Thức ăn:<span class="text-success">còn</span></span></div></div></div><div class="card" id="amount-card"><div class="card-body"><div class="card-title d-flex"><h4>Lượng thức ăn</h4><span class="ms-2 me-2">|</span><span class="text-muted" id="amount-text"></span></div><input type="range" class="form-range" id="amount" step="1" min="1" max="20" value="2"></div></div><div class="card d-flex" section-view="schedule" id="schedule-card"><div class="card-logo-container"><div class="card-logo"><i class="mt-md-1 fa-3x fal fa-clock" style="margin-top:0"></i></div></div><div class="card-content w-100"><div class="w-100 ms-3 ms-md-1"><h4>Hẹn giờ cho ăn</h4></div><span class="expand-icon me-3"></span></div></div><div class="card" section-view="history" id="history-card" style="max-height:400px;overflow:hidden"><div class="card-body pb-0"><div class="card-title d-flex justify-content-between mb-3"><h4>Lịch sử cho ăn</h4><span class="expand-icon"></span></div><div class="timeline-container"></div></div></div></section><section class="d-none fade show" id="schedule"><div class="modal fade" id="add-schedule-modal" tabindex="-1" aria-hidden="true"><div class="modal-dialog modal-fullscreen-sm-down"><div class="modal-content px-2"><div class="modal-header"><a class="btn-close-modal text-black" data-bs-dismiss="modal"><i class="fa-2x fal fa-times text-muted"></i></a></div><div class="modal-body pt-0"><h2 class="mb-5">Thêm hẹn giờ mới</h2><div class="fs-4"><div class="d-flex justify-content-between align-items-center mb-4"><label for="schedule-time" class="form-label w-100 fw-bold">Thời gian</label><input class="form-control fs-4" style="width:150px" type="time" id="schedule-time"></div><div class="mb-4"><label for="schedule-amount" class="form-label mb-1 fw-bold">Lượng thức ăn</label><input type="range" class="form-range" id="schedule-amount" step="1" min="1" max="20" value="2"></div><div class="d-flex justify-content-between align-items-center mb-4"><span class="fs-4 fw-bold">Lặp lại</span><label for="schedule-repeat" class="form-label">Chọn</label><select multiple="multiple" class="visually-hidden" style="z-index:-1" id="schedule-repeat"><option value="0">Thứ 2</option><option value="1">Thứ 3</option><option value="2">Thứ 4</option><option value="3">Thứ 5</option><option value="4">Thứ 6</option><option value="5">Thứ 7</option><option value="6">Chủ nhật</option></select></div></div></div><div class="modal-footer"><button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Hủy</button><button type="button" class="btn btn-primary" id="save-task">Lưu</button></div></div></div></div><div class="modal fade" id="task-detail-modal" tabindex="-1" aria-hidden="true"><div class="modal-dialog modal-fullscreen-sm-down"><div class="modal-content px-2"><div class="modal-header"><a class="btn-close-modal text-black" data-bs-dismiss="modal"><i class="fa-2x fal fa-times text-muted"></i></a></div><div class="modal-body pt-0"><h2 class="mb-5">Chỉnh sửa hẹn giờ</h2><div class="fs-4"><div class="d-flex justify-content-between align-items-center mb-4"><label for="schedule-time" class="form-label w-100 fw-bold">Thời gian</label><input class="form-control fs-4" style="width:150px" type="time" id="task-time"></div><div class="mb-4"><label for="schedule-amount" class="form-label mb-1 fw-bold">Lượng thức ăn</label><input type="range" class="form-range" id="task-amount" step="1" min="1" max="20" value="2"></div><div class="d-flex justify-content-between align-items-center mb-4"><span class="fs-4 fw-bold">Lặp lại</span><label for="task-repeat" class="form-label">Chọn</label><select multiple="multiple" class="visually-hidden" style="z-index:-1" id="task-repeat"><option value="0">Thứ 2</option><option value="1">Thứ 3</option><option value="2">Thứ 4</option><option value="3">Thứ 5</option><option value="4">Thứ 6</option><option value="5">Thứ 7</option><option value="6">Chủ nhật</option></select></div><div class="d-flex justify-content-center w-100"><button class="btn btn-danger rounded-pill" id="delete-task" task-id="">Xóa hẹn giờ</button></div></div></div><div class="modal-footer"><button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Hủy</button><button type="button" class="btn btn-primary" id="save-task">Lưu</button></div></div></div></div><div class="back-to-home"></div><div class="d-flex justify-content-center mb-5"><h2>Hẹn giờ</h2></div><div class="btn-add"></div><div class="task-container"></div></section><section class="d-none fade show" id="history"><div class="back-to-home"></div><div class="d-flex justify-content-center"><h2>Lịch sử cho ăn</h2></div><div class="d-flex justify-content-center mt-4"><div class="timeline-container" style="width:150px"></div></div></section></div></div></div><script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz" crossorigin="anonymous"></script><script src="https://code.jquery.com/jquery-3.1.1.min.js"></script><script>var TASKS = []
var wsLastCmd = '';
var ws = new WebSocket(`ws://${window.location.host}:81/`);
ws.addEventListener("message", (e) => {
    console.log(e.data);
    if (e.data == 'FEEDED') {
        $('#feed-card').removeClass('disabled');
        $('#feed-text').text('Cho ăn');
        $('.loading').addClass('d-none');
    }
    if (e.data.startsWith('TASK_ADDED') || e.data.startsWith('TASK_REMOVED')) {
        getTasks()
    }
});</script><script>function timeStr(h, m) {
    return `${h < 10 ? '0' + h : h}:${m < 10 ? '0' + m : m}`
}

function showSection(section) {
    $('section').removeClass('show');
    setTimeout(() => {
        $('section').addClass('d-none');
        $(`section#${section}`).removeClass('d-none');
    }, 200);
    setTimeout(() => {
        $(`section#${section}`).addClass('show');
    }, 300);
}

function getLogs() {
    $('.timeline-container').empty()
    $.ajax('/logs').done((data) => {
        console.log(data)
        const ndata = []
        data.split('\n').forEach((line) => {
            if (line) {
                let [time, amount] = line.split(' ');
                time = parseInt(time);
                if (time < 1000000000)
                    return
                ndata.push({ t: time - 7 * 3600, a: amount })
            }
        })
        ndata.sort((a, b) => b.t - a.t).forEach((log) => {
            const date = new Date(log.t * 1000);
            const time = `<span class="text-muted">${date.getDate()}/${date.getMonth() + 1}</span> ${date.toLocaleTimeString('vi-VN').slice(0, 5)}`;
            $('.timeline-container').append(`
                <div class="timeline">
                    <div class="time">${time}</div>
                    <div class="activity">${log.a}</div>
                </div>
            `)
        })
    })
}

function parseTask(str) {
    console.log(str)
    let [id, h, m, rstr, amount, state] = str.split('|');
    let days = ['Thứ 2', 'Thứ 3', 'Thứ 4', 'Thứ 5', 'Thứ 6', 'Thứ 7', 'Chủ nhật'];
    let repeat = [];
    rstr.split('').forEach((x, i) => {
        if (x === '1')
            repeat.push(days[i])
    })
    if (repeat.length == 7)
        repeat = 'Hằng ngày'
    else if (repeat.length == 1 && repeat[0] === 'Chủ nhật')
        repeat = 'Mỗi Chủ nhật'
    else
        repeat = `Mỗi thứ ${repeat.join(', ')}`
    return { id, h, m, repeat, amount, state }
}

function getTasks() {
    $('.task-container').empty()
    let tasks = []
    $.ajax(`/schedules`).done((data) => {
        console.log(data)
        if (!data) return
        if (!data.includes('\n'))
            tasks.push(parseTask(data))
        else
            data.split('\n').map((d) => {
                if (!d) return
                tasks.push(parseTask(d))
            })
        if (tasks.length)
            TASKS = tasks
            tasks.forEach((task) => {
                let time = timeStr(task.h, task.m)
                let checked = task.state == 1 ? 'checked' : ''
                $('.task-container').append(`
                <div class="card card-task mb-3" task-id="${task.id}">
                    <div class="card-body d-flex align-items-center">
                        <div class="d-flex flex-column w-100">
                            <span class="fs-2">${time}</span>
                            <span class="text-muted">${task.amount}g | ${task.repeat}</span>
                        </div>
                        <div class="form-check form-switch">
                            <input class="form-check-input form-check-lg" type="checkbox" role="switch" ${checked}>
                        </div>
                    </div>
                </div>
            `)
            })
    })
}

function renderTaskModal(taskId) {
    const task = TASKS.find(t => t.id == taskId)
    $('#task-time').val(timeStr(task.h, task.m))
    $('#task-amount').val(task.amount)
    // $('#task-repeat').val(task.repeat)
    $('label[for="task-repeat"]').text(task.repeat)
    $('#delete-task').attr('task-id', task.id)
    $('#task-detail-modal').modal('show');
}

// Section view
$('[section-view]').click(function () {
    showSection($(this).attr('section-view'));
})

// Back to home
$('.back-to-home').click(() => {
    showSection('home');
})

// Amount
$('#amount').on('input', function () {
    $('#amount-text').text(`${$(this).val()}g`);
})
$('#amount').change(() => {
    $('.loading .message').text(`Đã đặt lượng thức ăn: ${$('#amount').val()}g`);
    $('.loading').removeClass('d-none');
    $('#amount-card').addClass('disabled');
    setTimeout(() => {
        $('.loading').addClass('d-none');
        $('#amount-card').removeClass('disabled');
    }, 3000);
})
$('#amount').trigger('input');


// Feed
$('#feed-card').click(() => {
    let a = $('#amount').val();
    ws.send(`#FEED ${a}`);
    $('#feed-text').text('Đang cho ăn...');
    $('#feed-card').addClass('disabled');
    $('.loading').removeClass('d-none');
    $('.loading .message').text(`Đang cho ăn ${a}g`);
})

// Add schedule modal
$('.btn-add').click(() => {
    $('#add-schedule-modal').modal('show');
})


// Add Schedule
$('#schedule-amount').on('input', function () {
    const v = $(this).val();
    const $l = $('#schedule-amount').parent().find('label')
    if (v)
        $l.text(`Lượng thức ăn: ${v}g`)
    else
        $l.text('Lượng thức ăn')
})
$('#schedule-amount').trigger('input')

$('#schedule-repeat').change(() => {
    const $l = $('#schedule-repeat').parent().find('label')
    let selected = $('#schedule-repeat').val()
    if (selected && selected.length) {
        selected = selected.sort((a, b) => a - b).map(x => String(parseInt(x) + 2))
        if (selected.length === 7)
            return $l.text(`Hằng ngày`)
        if (selected.length === 1 && selected[0] === '8')
            return $l.text(`Mỗi Chủ nhật`)
        selected = selected.join(', ').replace(/8/g, 'Chủ nhật')
        $l.text(`Mỗi thứ ${selected}`)
    } else {
        $l.text('Chọn')
    }
})

// Add task
$('#save-task').click(() => {
    const time = $('#schedule-time').val();
    const h = parseInt(time.slice(0, 2));
    const m = parseInt(time.slice(3, 5));
    const amount = $('#schedule-amount').val();
    const repeat = $('#schedule-repeat').val();
    let rstr = ''
    for (let i = 0; i < 7; i++) {
        if (repeat.includes(String(i)))
            rstr += '1'
        else
            rstr += '0'
    }
    $('#schedule-time').val('')
    $('#schedule-amount').val(2)
    $('#schedule-repeat').val('')
    $('#add-schedule-modal').modal('hide');
    ws.send(`#TASK ${Math.floor(Math.random() * 100)}|${h}|${m}|${rstr}|${amount}|1|0`);
})

// Delete task
$('#delete-task').click(() => {
    const taskId = $('#delete-task').attr('task-id');
    ws.send(`#RMTASK ${taskId}`);
    $('#task-detail-modal').modal('hide');
})

// Task detail
$('.task-container').on('click', '.card-task', function () {
    renderTaskModal($(this).attr('task-id'))
})</script><script>getLogs(),getTasks()</script></body></html>
)html";