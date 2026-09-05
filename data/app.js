"use strict";

let clocks = [];
let timezones = [];

async function readJson(url)
{
    const response = await fetch(url);

    if (!response.ok)
        throw new Error(`${url}: HTTP ${response.status}`);

    return response.json();
}

async function showPage(pageName, button)
{
    document.querySelectorAll(".page").forEach(p => p.classList.remove("active"));
    document.querySelectorAll(".tabButton").forEach(b => b.classList.remove("active"));
    document.getElementById(pageName).classList.add("active");
    button.classList.add("active");

    switch (pageName)
    {
        case "home":
            await loadHomeControls();
            break;

        case "network":
            await loadNetwork();
            break;

        case "clocks":
            if (timezones.length === 0)
                await loadTimezones();
            await loadClocks();
            break;

        case "display":
            await loadDisplay();
            break;

        case "system":
            await loadSystem();
            break;

        case "meteo":
            await loadSensor();
            await loadMeteo();
            break;
    }
}

function toggleDHCPFields()
{
    const dhcp = document.getElementById("wifi_dhcp").checked;
    document.getElementById("staticIPFields").style.display = dhcp ? "none" : "block";
}

//==========================================================
// Home: volume quick control
//==========================================================

let pendingHomeVolume = null;
let homeVolumeTimer   = null;

async function loadHomeControls()
{
    try
    {
        const aud = await readJson("/api/audio");
        const v = Number(aud.volume);
        if (!isNaN(v))
        {
            const vs = document.getElementById("homeVolume");
            vs.value = v;
            document.getElementById("homeVolumeValue").textContent = v;
        }
        document.getElementById("saluteSound").checked = aud.saluteSound !== false;
    }
    catch (e)
    {
        console.error(e);
    }

    try
    {
        const sens = await readJson("/api/sensor");
        const t = Number(sens.tempOffset);
        if (!isNaN(t))
        {
            document.getElementById("tempOffset").value = t;
            document.getElementById("tempOffsetValue").textContent =
                t.toFixed(1);
        }
        const h = Number(sens.humiOffset);
        if (!isNaN(h))
        {
            document.getElementById("humiOffset").value = h;
            document.getElementById("humiOffsetValue").textContent =
                h.toFixed(1);
        }
    }
    catch (e)
    {
        console.error(e);
    }
}

function onHomeVolumeInput()
{
    const v = Number(document.getElementById("homeVolume").value);
    document.getElementById("homeVolumeValue").textContent = v;
    pendingHomeVolume = v;

    if (homeVolumeTimer !== null)
        clearTimeout(homeVolumeTimer);

    homeVolumeTimer = setTimeout(sendHomeVolume, 200);
}

async function sendHomeVolume()
{
    if (pendingHomeVolume === null)
        return;

    const v = pendingHomeVolume;

    try
    {
        await fetch("/api/audio",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ volume: v })
        });
    }
    catch (e)
    {
        console.error(e);
    }
}

//==========================================================
// Home: temperature correction quick control
//==========================================================

let pendingTempOffset = null;
let tempOffsetTimer   = null;

function onTempOffsetInput()
{
    const v = Number(document.getElementById("tempOffset").value);
    if (isNaN(v))
        return;

    document.getElementById("tempOffsetValue").textContent = v.toFixed(1);
    pendingTempOffset = v;

    if (tempOffsetTimer !== null)
        clearTimeout(tempOffsetTimer);

    tempOffsetTimer = setTimeout(sendTempOffset, 400);
}

async function sendTempOffset()
{
    if (pendingTempOffset === null)
        return;

    const v = pendingTempOffset;

    try
    {
        await fetch("/api/sensor",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ tempOffset: v })
        });
    }
    catch (e)
    {
        console.error(e);
    }
}

//==========================================================
// Home: humidity correction quick control
//==========================================================

let pendingHumiOffset = null;
let humiOffsetTimer   = null;

function onHumiOffsetInput()
{
    const v = Number(document.getElementById("humiOffset").value);
    if (isNaN(v))
        return;

    document.getElementById("humiOffsetValue").textContent = v.toFixed(1);
    pendingHumiOffset = v;

    if (humiOffsetTimer !== null)
        clearTimeout(humiOffsetTimer);

    humiOffsetTimer = setTimeout(sendHumiOffset, 400);
}

async function sendHumiOffset()
{
    if (pendingHumiOffset === null)
        return;

    const v = pendingHumiOffset;

    try
    {
        await fetch("/api/sensor",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ humiOffset: v })
        });
    }
    catch (e)
    {
        console.error(e);
    }
}

async function onSaluteSoundChange()
{
    const enabled = document.getElementById("saluteSound").checked;

    try
    {
        await fetch("/api/audio",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ saluteSound: enabled })
        });
    }
    catch (e)
    {
        console.error(e);
    }
}

//==========================================================
// Network
//==========================================================

async function loadNetwork()
{
    try
    {
        const cfg = await readJson("/api/network");
        document.getElementById("wifi_ssid").value = cfg.ssid || "";
        document.getElementById("wifi_password").value = "";
        document.getElementById("wifi_dhcp").checked =
            cfg.dhcp !== undefined ? cfg.dhcp : true;
        document.getElementById("wifi_ip").value = cfg.ip || "192.168.1.100";
        document.getElementById("wifi_subnet").value = cfg.subnet || "255.255.255.0";
        document.getElementById("wifi_gateway").value = cfg.gateway || "192.168.1.1";
        document.getElementById("wifi_dns").value = cfg.dns || "8.8.8.8";
        toggleDHCPFields();
    }
    catch (e)
    {
        console.error(e);
        alert("Network read error");
    }
}

async function saveNetwork()
{
    const cfg =
    {
        ssid: document.getElementById("wifi_ssid").value,
        dhcp: document.getElementById("wifi_dhcp").checked,
        ip: document.getElementById("wifi_ip").value,
        subnet: document.getElementById("wifi_subnet").value,
        gateway: document.getElementById("wifi_gateway").value,
        dns: document.getElementById("wifi_dns").value
    };

    const password = document.getElementById("wifi_password").value;
    if (password.length > 0)
        cfg.password = password;

    try
    {
        const response = await fetch("/api/network",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(cfg)
        });
        alert(response.ok ? "Network saved." : "Save error.");
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

async function loadSystem()
{
    try
    {
        const sys = await readJson("/api/system");
        document.getElementById("chipModel").textContent = sys.chipModel;
        document.getElementById("chipRevision").textContent = sys.chipRevision;
        document.getElementById("cpuMHz").textContent = `${sys.cpuMHz} MHz`;
        document.getElementById("flashSize").textContent = sys.flashSize;
        document.getElementById("freeHeap").textContent = sys.freeHeap;
        document.getElementById("freePsram").textContent = sys.freePsram;
    }
    catch (e)
    {
        console.error(e);
        alert("System read error");
    }

    await loadArchive();
}

//==========================================================
// Meteo archive (automatic 14-day backups)
//==========================================================

// Archive file names downloaded to the computer in this page
// session. When every stored archive has been downloaded, the
// device is told to clear the on-screen "archive needed"
// reminder (POST /api/archive/downloaded).
let archDownloaded = new Set();

async function loadArchive()
{
    try
    {
        await refreshMeteoTzOffset();

        const arch = await readJson("/api/archive");

        const lastEl = document.getElementById("archiveLast");
        if (arch.last > 0)
            lastEl.textContent = "Last archive: " + formatDateTime(arch.last);
        else
            lastEl.textContent = "Last archive: never";

        const remindEl = document.getElementById("archiveRemindState");
        if (remindEl)
        {
            remindEl.textContent = arch.remind
                ? "Reminder active: archive memory is full. Download the archives to your computer."
                : "";
        }

        const cb = document.getElementById("archiveReminderEnabled");
        if (cb && document.activeElement !== cb)
            cb.checked = arch.reminderEnabled !== false;

        const listEl = document.getElementById("archiveFiles");
        const files = Array.isArray(arch.files) ? arch.files : [];

        if (files.length === 0)
        {
            listEl.textContent = "No archives yet.";
            return;
        }

        let html = "";
        for (const f of files)
        {
            const kb = (f.size / 1024).toFixed(1);
            html += `<p style="margin:4px 0;">
<a href="/archive/${encodeURIComponent(f.name)}" download
   style="color:#fff;" onclick="downloadArchive(event, '${f.name}')">${f.name}</a>
&nbsp;(${kb} KB)</p>`;
        }
        listEl.innerHTML = html;
    }
    catch (e)
    {
        console.error(e);
    }
}

// Download one archive to the computer via fetch (so we know it
// really completed) and acknowledge when everything is saved.
// With the "Download archives as CSV" checkbox enabled the file
// is converted to CSV by the device (/archive/<name>.csv).
async function downloadArchive(event, name)
{
    if (event)
        event.preventDefault();

    const csvEl = document.getElementById("archiveCsvMode");
    const asCsv = csvEl && csvEl.checked;
    const saveName = asCsv ? name.replace(/\.bin$/, ".csv") : name;

    try
    {
        const response = await fetch("/archive/"
            + encodeURIComponent(name.replace(/\.bin$/, ""))
            + (asCsv ? ".csv" : ""));
        if (!response.ok)
            throw new Error(`HTTP ${response.status}`);

        const blob = await response.blob();
        const url  = URL.createObjectURL(blob);
        const a    = document.createElement("a");
        a.href     = url;
        a.download = saveName;
        document.body.appendChild(a);
        a.click();
        a.remove();
        URL.revokeObjectURL(url);

        archDownloaded.add(name);
        await acknowledgeArchives();
    }
    catch (e)
    {
        console.error(e);
        alert(`Download failed: ${name}`);
    }
}

// Download every stored archive sequentially, then acknowledge.
async function downloadAllArchives()
{
    try
    {
        const arch = await readJson("/api/archive");
        const files = Array.isArray(arch.files) ? arch.files : [];

        if (files.length === 0)
        {
            alert("No archives stored yet.");
            return;
        }

        for (const f of files)
            await downloadArchive(null, f.name);

        await acknowledgeArchives();
        await loadArchive();
        alert(`Saved ${files.length} archive file(s) to your computer.`);
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

// Tell the device that all currently stored archives have been
// downloaded; it clears the on-screen reminder.
async function acknowledgeArchives()
{
    try
    {
        const arch = await readJson("/api/archive");
        const files = Array.isArray(arch.files) ? arch.files : [];

        const all = files.every(f => archDownloaded.has(f.name));
        if (!all)
            return;

        await fetch("/api/archive/downloaded", { method: "POST" });
        archDownloaded.clear();
        await loadArchive();
    }
    catch (e)
    {
        console.error(e);
    }
}

async function toggleArchiveReminder()
{
    const enabled = document.getElementById("archiveReminderEnabled").checked;

    try
    {
        await fetch("/api/archive/reminder",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ enabled: enabled })
        });
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

async function archiveNow()
{
    if (!confirm("Create a meteo archive now?"))
        return;

    try
    {
        const response = await fetch("/api/archive/now", { method: "POST" });
        const data = await response.json();

        if (data.status === "ok")
            alert(`Archive created: ${data.fileName}`);
        else
            alert("Error creating archive.");

        await loadArchive();
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

async function loadClocks()
{
    try
    {
        clocks = await readJson("/api/clocks");

        if (!Array.isArray(clocks.clocks))
            throw new Error("Invalid clocks response");

        drawClocks();
    }
    catch (e)
    {
        console.error(e);
        alert("Clock read error");
    }
}

async function loadTimezones()
{
    try
    {
        timezones = await readJson("/api/timezones");
    }
    catch (e)
    {
        console.error(e);
        alert("Timezones read error");
        throw e;
    }
}

function minutesToTime(value)
{
    const minutes = Number(value) || 0;
    const hours = Math.floor(minutes / 60).toString().padStart(2, "0");
    const mins = (minutes % 60).toString().padStart(2, "0");
    return `${hours}:${mins}`;
}

function timeToMinutes(value)
{
    const [hours, minutes] = value.split(":").map(Number);
    return hours * 60 + minutes;
}

function drawClocks()
{
    let html = "";

    for (let i = 0; i < clocks.clocks.length; i++)
    {
        const clock = clocks.clocks[i];
        let options = "";

        for (const tz of timezones)
        {
            const sign = tz.utcHour >= 0 ? "+" : "-";
            const hours = Math.abs(tz.utcHour).toString().padStart(2, "0");
            const minutes = tz.utcMinute.toString().padStart(2, "0");
            const selected = clock.city === tz.nameEN ? " selected" : "";
            const label = `${tz.nameEN} / ${tz.nameUA} (UTC${sign}${hours}:${minutes})`;
            options += `<option value="${tz.nameEN}" data-index="${tz.index}"${selected}>${label}</option>`;
        }

        html += `
<div class="clockCard">
    <div class="clockTitle">Clock ${i + 1}</div>

    <label for="city${i}">City & Timezone</label>
    <select id="city${i}">${options}</select>

    <label>Working hours</label>
    <div class="workTimeRow">
        <input type="time" id="workStart${i}" value="${minutesToTime(clock.workStart)}">
        <span>—</span>
        <input type="time" id="workEnd${i}" value="${minutesToTime(clock.workEnd)}">
    </div>

    <div class="checkboxRow">
        <input type="checkbox" id="workSaturday${i}"${clock.workSaturday ? " checked" : ""}>
        <label for="workSaturday${i}" class="checkboxLabel">Saturday is a workday</label>
    </div>
    <div class="checkboxRow">
        <input type="checkbox" id="workSunday${i}"${clock.workSunday ? " checked" : ""}>
        <label for="workSunday${i}" class="checkboxLabel">Sunday is a workday</label>
    </div>
</div>`;
    }

    document.getElementById("clockContainer").innerHTML = html;
}

async function saveClocks()
{
    for (let i = 0; i < clocks.clocks.length; i++)
    {
        const select = document.getElementById(`city${i}`);
        const option = select.options[select.selectedIndex];
        const clock = clocks.clocks[i];

        clock.city = select.value;
        clock.timezone = Number(option.dataset.index);
        clock.workStart = timeToMinutes(document.getElementById(`workStart${i}`).value);
        clock.workEnd = timeToMinutes(document.getElementById(`workEnd${i}`).value);
        clock.workSaturday = document.getElementById(`workSaturday${i}`).checked;
        clock.workSunday = document.getElementById(`workSunday${i}`).checked;
    }

    try
    {
        const response = await fetch("/api/clocks",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(clocks)
        });
        alert(response.ok ? "Clocks saved." : "Save error.");
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

// "Home" is the default active tab, so its controls must be
// populated from the device on every page load.
window.onload = () => loadHomeControls();

//==========================================================
// Meteo data & charts
//==========================================================

let meteoData = null;
let sensorHasHumidity = true;

// Chart range filter (Unix seconds, null = unbounded). When null on
// loadMeteo() the inputs are pre-filled from the received data range.
let meteoFrom = null;
let meteoTo   = null;

// Meteo times are displayed in the timezone of the FIRST clock
// (index 0), not in the browser's local timezone. The offset
// (seconds, incl. DST) comes from /api/clocks -> clocks[0].utcOffset.
let meteoTzOffsetSec = 0;

async function refreshMeteoTzOffset()
{
    try
    {
        const cfg = await readJson("/api/clocks");
        const first = (cfg.clocks && cfg.clocks[0]) || null;
        if (first && Number.isFinite(first.utcOffset))
            meteoTzOffsetSec = first.utcOffset;
    }
    catch (e)
    {
        console.error(e);
    }
}

// Date helper: returns a Date whose UTC getters yield the wall
// time of the first clock for the given Unix timestamp.
function meteoShiftedDate(unixSec)
{
    return new Date((unixSec + meteoTzOffsetSec) * 1000);
}

//----------------------------------------------------------
// Sensor info (auto-detected: BME280 / BMP280)
//----------------------------------------------------------

async function loadSensor()
{
    try
    {
        const s = await readJson("/api/sensor");
        sensorHasHumidity = !!s.hasHumidity;

        const info = document.getElementById("sensorInfo");
        if (info)
        {
            const active = s.active || "none";
            let text;
            if (active === "none")
            {
                text = "No sensor detected. Check wiring (SDA/SCL/power).";
            }
            else
            {
                const hum = sensorHasHumidity
                    ? "with humidity"
                    : "no humidity channel";
                text = `Detected: ${active} \u2014 ${hum}.`;
            }
            info.textContent = text;
        }

        applyHumidityVisibility();
    }
    catch (e)
    {
        console.error(e);
    }
}

function applyHumidityVisibility()
{
    const show = sensorHasHumidity;
    const stat = document.getElementById("meteoHumStat");
    const card = document.getElementById("meteoHumCard");
    if (stat) stat.style.display = show ? "" : "none";
    if (card) card.style.display = show ? "" : "none";
}

//----------------------------------------------------------
// Chart state
//----------------------------------------------------------
const charts = [
    { id: "chartTemp", canvas: null, values: [], color: "#EF5350", unit: "\u00B0C" },
    { id: "chartPres", canvas: null, values: [], color: "#4FC3F7", unit: "hPa" },
    { id: "chartHum",  canvas: null, values: [], color: "#66BB6A", unit: "%" }
];
const PAD = { L: 46, R: 12, T: 10, B: 18 };
let meteoTime = [];
let hoverT = null;
let chartsBound = false;

//----------------------------------------------------------
// Range filter helpers
//----------------------------------------------------------

function meteoUrl()
{
    let url = "/api/meteo";
    const q = [];
    if (meteoFrom !== null) q.push("from=" + meteoFrom);
    if (meteoTo   !== null) q.push("to="   + meteoTo);
    if (q.length) url += "?" + q.join("&");
    return url;
}

// "YYYY-MM-DDTHH:MM" in the FIRST clock's timezone, suitable for
// <input type="datetime-local">.
function toLocalDateTimeInput(unixSec)
{
    const d  = meteoShiftedDate(unixSec);
    const Y  = d.getUTCFullYear();
    const Mo = String(d.getUTCMonth() + 1).padStart(2, "0");
    const Da = String(d.getUTCDate()).padStart(2, "0");
    const H  = String(d.getUTCHours()).padStart(2, "0");
    const Mi = String(d.getUTCMinutes()).padStart(2, "0");
    return `${Y}-${Mo}-${Da}T${H}:${Mi}`;
}

// Parse a datetime-local string as a time in the FIRST clock's
// timezone into Unix seconds, or null when empty / invalid.
function fromLocalDateTimeInput(str)
{
    if (!str) return null;
    const m = str.match(/^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2})/);
    if (!m) return null;
    return Math.floor(
        Date.UTC(+m[1], +m[2] - 1, +m[3], +m[4], +m[5]) / 1000)
        - meteoTzOffsetSec;
}

// Read the inputs, store bounds, and reload. Swapped bounds are
// quietly swapped rather than producing an empty result.
function applyMeteoRange()
{
    let from = fromLocalDateTimeInput(document.getElementById("meteoFrom").value);
    let to   = fromLocalDateTimeInput(document.getElementById("meteoTo").value);
    if (from !== null && to !== null && from > to)
        [from, to] = [to, from];
    meteoFrom = from;
    meteoTo   = to;

    console.log("applyMeteoRange:", { from, to, url: meteoUrl() });

    const info = document.getElementById("meteoInfo");
    if (info) info.textContent = "Loading filtered data...";

    loadMeteo();
}

// Clear bounds and inputs, reload the full history.
function resetMeteoRange()
{
    meteoFrom = null;
    meteoTo   = null;
    const f = document.getElementById("meteoFrom");
    const t = document.getElementById("meteoTo");
    if (f) f.value = "";
    if (t) t.value = "";
    loadMeteo();
}

async function loadMeteo()
{
    await refreshMeteoTzOffset();

    const info = document.getElementById("meteoInfo");
    try
    {
        meteoData = await readJson(meteoUrl());
        meteoTime = meteoData.time || [];

        charts[0].values = meteoData.temp || [];
        charts[1].values = meteoData.pres || [];
        charts[2].values = meteoData.hum  || [];

        const count    = meteoData.count || 0;
        const interval = meteoData.interval || 300;

        if (!chartsBound)
        {
            charts[0].canvas = document.getElementById("chartTemp");
            charts[1].canvas = document.getElementById("chartPres");
            charts[2].canvas = document.getElementById("chartHum");
            setupChartEvents();
            chartsBound = true;
            window.addEventListener("resize", redrawAll);
        }

        if (count === 0 || meteoTime.length === 0)
        {
            const filtered = (meteoFrom !== null || meteoTo !== null);
            info.textContent = filtered
                ? "No samples in the selected range."
                : "No data yet. The first sample is recorded 5 minutes after the device starts.";
            document.getElementById("meteoCurTemp").textContent = "--";
            document.getElementById("meteoCurPres").textContent = "--";
            document.getElementById("meteoCurHum").textContent  = "--";
            for (const c of charts) c.values = [];
            hoverT = null;
            redrawAll();
            return;
        }

        document.getElementById("meteoCurTemp").textContent = lastValue(charts[0].values, "\u00B0C");
        document.getElementById("meteoCurPres").textContent = lastValue(charts[1].values, " hPa");
        document.getElementById("meteoCurHum").textContent  = lastValue(charts[2].values, " %");

        const shown     = meteoData.shown || meteoTime.length;
        const spanHours = (count * interval) / 3600;
        const days      = Math.round((spanHours / 24) * 10) / 10;
        info.textContent =
            `${count} samples \u00B7 ${shown} shown \u00B7 ~${days} days of history`;

        // Pre-fill empty inputs with the visible data range so the
        // user can narrow it without typing the whole timestamp.
        if (meteoTime.length > 0)
        {
            const fromInput = document.getElementById("meteoFrom");
            const toInput   = document.getElementById("meteoTo");
            if (fromInput && !fromInput.value)
                fromInput.value = toLocalDateTimeInput(meteoTime[0]);
            if (toInput && !toInput.value)
                toInput.value = toLocalDateTimeInput(meteoTime[meteoTime.length - 1]);
        }

        hoverT = null;
        redrawAll();
    }
    catch (e)
    {
        console.error(e);
        info.textContent = "Failed to load meteo data.";
        alert("Meteo read error");
    }
}

//----------------------------------------------------------
// Rendering
//----------------------------------------------------------

function redrawAll()
{
    for (const c of charts)
        drawSeries(c);
}

function setupChartEvents()
{
    for (const c of charts)
    {
        const cv = c.canvas;
        if (!cv) continue;
        cv.addEventListener("mousemove", onHover);
        cv.addEventListener("mouseleave", onLeave);
    }
}

function chartFromCanvas(cv)
{
    for (const c of charts) if (c.canvas === cv) return c;
    return null;
}

function clientXToPlot(cv, clientX)
{
    return clientX - cv.getBoundingClientRect().left;
}

function plotXToTime(c, px)
{
    const p = c._plot;
    const n = meteoTime.length;
    const tLo = n ? meteoTime[0] : 0;
    const tHi = n ? meteoTime[n - 1] : 1;
    if (!p) return tLo;
    const frac = (px - p.padL) / p.plotW;
    return tLo + frac * (tHi - tLo);
}

function nearestTimeIndex(t)
{
    const a = meteoTime;
    const n = a.length;
    if (n === 0) return -1;
    if (t <= a[0]) return 0;
    if (t >= a[n - 1]) return n - 1;
    let lo = 0, hi = n - 1;
    while (lo < hi)
    {
        const mid = (lo + hi) >> 1;
        if (a[mid] < t) lo = mid + 1; else hi = mid;
    }
    if (lo > 0 && Math.abs(a[lo - 1] - t) < Math.abs(a[lo] - t)) return lo - 1;
    return lo;
}

//----------------------------------------------------------
// Hover tooltip
//----------------------------------------------------------

function onHover(e)
{
    if (meteoTime.length === 0) return;
    const c = chartFromCanvas(e.currentTarget);
    if (!c || !c._plot) return;
    const px = clientXToPlot(c.canvas, e.clientX);
    if (px < c._plot.padL || px > c._plot.padL + c._plot.plotW)
    {
        if (hoverT !== null) { hoverT = null; redrawAll(); }
        hideTooltip();
        return;
    }
    const t   = plotXToTime(c, px);
    const idx = nearestTimeIndex(t);

    // Tooltip follows the cursor on every move, but the chart is
    // redrawn only when the snapped data point changes (avoids
    // constant redraws that make the chart visually rescale).
    const v       = idx >= 0 ? c.values[idx] : null;
    const valStr  = (v === null || v === undefined || isNaN(v))
        ? "\u2014"
        : `${v} ${c.unit}`;
    const timeStr = formatDateTime(idx >= 0 ? meteoTime[idx] : t);
    showTooltip(e.clientX, e.clientY,
        `<div class="ttTime">${timeStr}</div>` +
        `<div class="ttVal" style="color:${c.color}">${valStr}</div>`);

    const newHover = idx >= 0 ? meteoTime[idx] : t;
    if (newHover !== hoverT)
    {
        hoverT = newHover;
        redrawAll();
    }
}

function onLeave()
{
    if (hoverT !== null) { hoverT = null; redrawAll(); }
    hideTooltip();
}

function showTooltip(clientX, clientY, html)
{
    let el = document.getElementById("chartTooltip");
    if (!el)
    {
        el = document.createElement("div");
        el.id = "chartTooltip";
        document.body.appendChild(el);
    }
    el.innerHTML = html;
    el.style.display = "block";
    const pad = 14;
    let x = clientX + pad;
    let y = clientY + pad;
    if (x + el.offsetWidth > window.innerWidth)  x = clientX - el.offsetWidth - pad;
    if (y + el.offsetHeight > window.innerHeight) y = clientY - el.offsetHeight - pad;
    el.style.left = x + "px";
    el.style.top  = y + "px";
}

function hideTooltip()
{
    const el = document.getElementById("chartTooltip");
    if (el) el.style.display = "none";
}

function lastValue(arr, unit)
{
    for (let i = arr.length - 1; i >= 0; i--)
    {
        if (arr[i] !== null && arr[i] !== undefined && !isNaN(arr[i]))
            return `${arr[i]} ${unit}`.trim();
    }
    return "--";
}

function drawSeries(chart)
{
    const canvas = chart.canvas;
    const values = chart.values;
    const color  = chart.color;
    if (!canvas) return;
    // Skip hidden charts (e.g. humidity card when sensor is BMP280).
    if (canvas.offsetParent === null) return;

    const ctx = canvas.getContext("2d");

    // CSS pixel size; scale for device pixel ratio (crisp lines).
    const cssW = canvas.clientWidth || canvas.parentElement.clientWidth || 600;

    // Cache the logical height: canvas.height is a reflected
    // attribute, so after the first draw getAttribute("height")
    // returns the backing-store size (cssH*dpr). Re-reading it
    // would multiply the height by dpr on every redraw.
    if (!chart.cssH)
        chart.cssH = parseInt(canvas.getAttribute("height")) || 160;
    const cssH = chart.cssH;

    const dpr = window.devicePixelRatio || 1;

    // Reallocate the backing store only when the CSS size actually
    // changes; otherwise moving the mouse would reset/clear the
    // canvas every event and make the chart visually rescale.
    const needW = Math.round(cssW * dpr);
    const needH = Math.round(cssH * dpr);
    if (canvas.width !== needW || canvas.height !== needH)
    {
        canvas.width  = needW;
        canvas.height = needH;
        canvas.style.height = cssH + "px";
    }
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

    ctx.clearRect(0, 0, cssW, cssH);

    const padL = PAD.L, padR = PAD.R, padT = PAD.T, padB = PAD.B;
    const plotW = cssW - padL - padR;
    const plotH = cssH - padT - padB;
    chart._plot = { padL, padR, padT, padB, plotW, plotH, cssW, cssH };

    // Background + frame.
    ctx.fillStyle = "#263238";
    ctx.fillRect(0, 0, cssW, cssH);
    ctx.strokeStyle = "#37474F";
    ctx.lineWidth = 1;
    ctx.strokeRect(padL, padT, plotW, plotH);

    if (values.length === 0 || meteoTime.length === 0)
    {
        ctx.fillStyle = "#90A4AE";
        ctx.font = "14px Arial";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillText("No data", cssW / 2, cssH / 2);
        return;
    }

    const tLo = meteoTime[0];
    const tHi = meteoTime[meteoTime.length - 1];
    const tRange = (tHi - tLo) || 1;

    // Min/max over all non-null values.
    let min = Infinity, max = -Infinity;
    for (let i = 0; i < values.length; i++)
    {
        const v = values[i];
        if (v === null || v === undefined || isNaN(v)) continue;
        if (v < min) min = v;
        if (v > max) max = v;
    }
    if (!isFinite(min)) { min = 0; max = 1; }
    if (max - min < 0.001) { min -= 1; max += 1; }
    const range = max - min;
    min -= range * 0.08;
    max += range * 0.08;

    const xOf = t => padL + ((t - tLo) / tRange) * plotW;
    const yOf = v => padT + plotH - ((v - min) / (max - min)) * plotH;

    // Horizontal gridlines + y labels.
    ctx.strokeStyle = "#37474F";
    ctx.fillStyle = "#90A4AE";
    ctx.font = "11px Arial";
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    const ticks = 4;
    for (let i = 0; i <= ticks; i++)
    {
        const v = min + (i / ticks) * (max - min);
        const y = yOf(v);
        ctx.beginPath();
        ctx.moveTo(padL, y);
        ctx.lineTo(padL + plotW, y);
        ctx.stroke();
        ctx.fillText(v.toFixed(Math.abs(max - min) < 10 ? 1 : 0), padL - 6, y);
    }

    // X labels (visible start / end).
    ctx.textBaseline = "top";
    ctx.textAlign = "left";
    ctx.fillText(formatDate(tLo), padL, cssH - padB + 3);
    ctx.textAlign = "right";
    ctx.fillText(formatDate(tHi), padL + plotW, cssH - padB + 3);

    // The series line.
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.lineJoin = "round";
    ctx.beginPath();
    let pen = false;
    for (let i = 0; i < values.length; i++)
    {
        const v = values[i];
        if (v === null || v === undefined || isNaN(v)) { pen = false; continue; }
        const x = xOf(meteoTime[i]);
        const y = yOf(v);
        if (!pen) { ctx.moveTo(x, y); pen = true; }
        else      { ctx.lineTo(x, y); }
    }
    ctx.stroke();

    // Crosshair (when hovering) or last-visible-point marker.
    if (hoverT !== null && hoverT >= tLo && hoverT <= tHi)
    {
        const cx = xOf(hoverT);
        ctx.save();
        ctx.strokeStyle = "rgba(255,255,255,0.35)";
        ctx.setLineDash([4, 3]);
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(cx, padT);
        ctx.lineTo(cx, padT + plotH);
        ctx.stroke();
        ctx.setLineDash([]);

        const idx = nearestTimeIndex(hoverT);
        if (idx >= 0)
        {
            const tt = meteoTime[idx];
            const v  = values[idx];
            if (tt >= tLo && tt <= tHi && v !== null && v !== undefined && !isNaN(v))
            {
                const dx = xOf(tt), dy = yOf(v);
                ctx.fillStyle = color;
                ctx.beginPath();
                ctx.arc(dx, dy, 4, 0, Math.PI * 2);
                ctx.fill();
                ctx.strokeStyle = "#FFFFFF";
                ctx.lineWidth = 1.5;
                ctx.beginPath();
                ctx.arc(dx, dy, 4, 0, Math.PI * 2);
                ctx.stroke();
            }
        }
        ctx.restore();
    }
    else
    {
        let lastIdx = -1;
        for (let i = values.length - 1; i >= 0; i--)
        {
            const t = meteoTime[i];
            if (t < tLo || t > tHi) continue;
            if (values[i] !== null && values[i] !== undefined && !isNaN(values[i]))
            { lastIdx = i; break; }
        }
        if (lastIdx >= 0)
        {
            ctx.fillStyle = color;
            ctx.beginPath();
            ctx.arc(xOf(meteoTime[lastIdx]), yOf(values[lastIdx]), 3, 0, Math.PI * 2);
            ctx.fill();
        }
    }
}

function formatDateTime(unixSec)
{
    const d  = meteoShiftedDate(unixSec);
    const dd = String(d.getUTCDate()).padStart(2, "0");
    const mm = String(d.getUTCMonth() + 1).padStart(2, "0");
    const hh = String(d.getUTCHours()).padStart(2, "0");
    const mi = String(d.getUTCMinutes()).padStart(2, "0");
    const ss = String(d.getUTCSeconds()).padStart(2, "0");
    return `${dd}.${mm} ${hh}:${mi}:${ss}`;
}

function formatDate(unixSec)
{
    const d = meteoShiftedDate(unixSec);
    const dd = String(d.getUTCDate()).padStart(2, "0");
    const mm = String(d.getUTCMonth() + 1).padStart(2, "0");
    const hh = String(d.getUTCHours()).padStart(2, "0");
    const mi = String(d.getUTCMinutes()).padStart(2, "0");
    return `${dd}.${mm} ${hh}:${mi}`;
}

async function triggerSalute()
{
    try
    {
        const response = await fetch("/api/salute", {
            method: "POST"
        });
        if (response.ok)
        {
            alert("Fireworks triggered on clock screen!");
        }
        else
        {
            alert("Error triggering fireworks.");
        }
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

//==========================================================
// Clear meteorological history
//==========================================================

async function clearMeteo()
{
    if (!confirm("Erase ALL retained meteo history? This cannot be undone."))
        return;

    try
    {
        const response = await fetch("/api/meteo/clear", {
            method: "POST"
        });
        if (response.ok)
        {
            alert("Meteo data cleared.");
        }
        else
        {
            alert("Error clearing meteo data.");
        }
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

//==========================================================
// Restore meteo history from a downloaded backup file.
//==========================================================

function onMeteoBackupChange()
{
    const input = document.getElementById("meteoBackupFile");
    const name  = document.getElementById("meteoBackupName");
    if (name)
        name.textContent = (input && input.files && input.files[0])
            ? input.files[0].name
            : "No file selected";
}

async function restoreMeteo()
{
    const input = document.getElementById("meteoBackupFile");
    if (!input || !input.files || !input.files[0])
    {
        alert("Select a backup file first.");
        return;
    }

    if (!confirm("Restore meteo history from the selected file? "
                 + "Samples collected after the backup are kept "
                 + "and appended after the archive."))
        return;

    try
    {
        const response = await fetch("/api/meteo/restore", {
            method: "POST",
            body: input.files[0]
        });
        if (response.ok)
        {
            const data = await response.json();
            alert("Restore complete. Samples stored: " + (data.count || 0));
        }
        else
        {
            alert("Error restoring meteo data.");
        }
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

//==========================================================
// Display settings (brightness + view mode)
//==========================================================

let pendingBrightness = null;
let brightnessSendTimer = null;

async function loadDisplay()
{
    try
    {
        const cfg = await readJson("/api/display");

        const brightness = Number(cfg.brightness);
        if (!isNaN(brightness))
        {
            const slider = document.getElementById("brightness");
            slider.value = brightness;
            document.getElementById("brightnessValue").textContent = brightness;
            pendingBrightness = brightness;
        }

        const mode = String(cfg.mode);
        for (const radio of document.getElementsByName("displayMode"))
        {
            radio.checked = (radio.value === mode);
        }
    }
    catch (e)
    {
        console.error(e);
        alert("Display read error");
    }
}

// Update the numeric label while dragging. The actual POST is
// debounced so we don't flood the device on every pixel of movement.
function onBrightnessInput()
{
    const slider = document.getElementById("brightness");
    const value = Number(slider.value);
    document.getElementById("brightnessValue").textContent = value;
    pendingBrightness = value;

    if (brightnessSendTimer !== null)
        clearTimeout(brightnessSendTimer);

    brightnessSendTimer = setTimeout(sendBrightnessLive, 250);
}

// Live preview: push brightness to the device without saving,
// so the user sees the effect while dragging.
async function sendBrightnessLive()
{
    if (pendingBrightness === null)
        return;

    const value = pendingBrightness;

    try
    {
        await fetch("/api/display",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ brightness: value })
        });
    }
    catch (e)
    {
        console.error(e);
    }
}

// "Apply" button: persist the current slider value.
async function saveBrightness()
{
    const value = Number(document.getElementById("brightness").value);

    try
    {
        const response = await fetch("/api/display",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ brightness: value })
        });
        alert(response.ok ? "Brightness saved." : "Save error.");
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

function onModeChange()
{
    const selected = document.querySelector('input[name="displayMode"]:checked');
    if (!selected)
        return;

    const mode = Number(selected.value);

    fetch("/api/display",
    {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ mode: mode })
    })
    .then(r =>
    {
        if (!r.ok)
            alert("Mode change failed.");
    })
    .catch(e =>
    {
        console.error(e);
        alert("Communication error.");
    });
}
