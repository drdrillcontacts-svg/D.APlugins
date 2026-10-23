# D.A Plugins — راهنمای کاربر

Development version 0.4 / 2026-10-23 / Source 1aaf67d9d271

## خوش‌آمد و معرفی باندل

### این باندل چه می‌کند

D.A Plugins مجموعه‌ای ده‌افکتی برای فضا، داینامیک، رنگ، هارمونیک و استریو است. روند یادگیری، صوت را تحلیل و پیش از تأیید، پیشنهاد پارامتر ارائه می‌کند. کار دستی همواره ممکن است. این دفترچه، پیاده‌سازی توسعه‌ای فعلی را توضیح می‌دهد، نه یک انتشار تجاری نهایی.

ابتدا شروع سریع و سپس فصل پلاگین موردنظر را بخوانید. مرجع کنترل‌ها شامل شناسهٔ واقعی، دامنهٔ دستی، مقدار پیش‌فرض و توضیح است. تنظیمات نمونه نقطهٔ شروع‌اند؛ پریست کارخانه‌ای یا راه‌حل تضمینی نیستند.

### پیش از شنیدن

پیش از تغییر حالت، بازخورد، Drive یا گین زیاد، صدای مانیتور را کم کنید. در میزبان هد‌روم نگه دارید و هم‌سطح مقایسه کنید. این مجموعه محافظ شنوایی، لیمیتر True Peak یا جایگزین لیمیتر ایمنی مستقل نیست. برای بیلد توسعه‌ای از نسخهٔ آزمایشی پروژه و پشتیبان استفاده کنید.

### روش خواندن راهنما

برچسب انگلیسی پارامتر با رابط فعلی مطابقت دارد. درصد، نسبت دامنه یا کنترل است و الزاماً نسبت بلندی ادراکی نیست. بعضی کنترل‌های نرمال‌شده مانند Feedback ممکن است اعشاری نمایش داده شوند؛ ۰٫۳۵ یعنی ۳۵٪. دامنهٔ ماژول همیشه با پنل ظاهری یکسان نیست.

## نصب، سازگاری و به‌روزرسانی

### دریافت بیلد مناسب

آرتیفکت قابل‌اعتماد متناسب با سیستم‌عامل و معماری پردازنده دریافت کنید. فایل ZIP طراحی رابط، نصب‌کنندهٔ پلاگین نیست. مخزن فعلاً نصب‌کنندهٔ تجاری امضاشده، مدیر فعال‌سازی یا انتشار نوتاریزه ندارد. خروجی CI تضمین کلی سازگاری با همهٔ DAWها نیست.

VST3 و Standalone هدف بیلد ویندوز، مک و لینوکس‌اند؛ AU فقط برای مک است. VST2، AAX و CLAP ارائه نمی‌شوند. میزبان نیازمند AAX این مجموعه را به‌صورت بومی AAX بارگذاری نمی‌کند. ماتریس کامل تأیید نسخه‌های میزبان منتشر نشده است.

### نصب دستی

DAW را ببندید. بسته‌های مناسب پلاگین را بدون تغییر ساختار داخلی استخراج و VST3 را در مسیر استاندارد موردقبول میزبان کپی کنید؛ مسیرهای رایج در ادامه آمده‌اند. در مک برای AU بستهٔ .component را کپی کنید. سپس میزبان را باز و فهرست پلاگین‌ها را دوباره اسکن کنید.

اگر سیستم بیلد بدون امضا را رد کرد، امنیت سیستم را سراسری غیرفعال یا فایل نامطمئن اجرا نکنید. فقط پس از بررسی منشأ، از روش رسمی تأیید همان فایل استفاده کنید یا بیلد امضاشده بخواهید.

### تنظیم میزبان و اجرای مستقل

افکت را روی ترک صوتی یا باس برگشت قرار دهید. ورودی و خروجی باید هر دو مونو یا هر دو استریو باشند. این‌ها ساز مجازی، افکت MIDI، پردازندهٔ فراگیر یا تبدیل‌کنندهٔ چیدمان مونو به استریو نیستند. ترک ساکت دادهٔ قابل‌استفاده‌ای برای Learn نمی‌دهد.

هدف Standalone از پوستهٔ JUCE به‌جای جلسهٔ DAW استفاده می‌کند. اگر پوسته تنظیمات دستگاه دارد، ورودی، خروجی و دستگاه مناسب انتخاب کنید و از برگشت صدای اسپیکر به میکروفن جلوگیری کنید. برای مسیریابی، اتومیشن و بازخوانی، DAW محیط پیشنهادی است.

### به‌روزرسانی یا حذف

پیش از جایگزینی، جلسه را پشتیبان بگیرید و بیلد قبلی را نگه دارید. میزبان را ببندید، بستهٔ متناظر را جایگزین، اسکن و نسخهٔ کپی جلسه را آزمایش کنید. مهاجرت وضعیت نمونهٔ اولیهٔ چهارپارامتری تضمین نیست. برای حذف، فقط بسته‌های نصب‌شدهٔ مربوط را بردارید، نه پوشه‌های مشترک میزبان.

## شروع سریع و مسیر امن سیگنال

### شنیدن اولیه در شش گام

۱. نسخه‌ای از جلسه ذخیره و مانیتور را کم کنید. پلاگینی انتخاب کنید که یک مسئلهٔ مشخص را حل کند. ۲. بخش نمایندهٔ اجرا را به آن برسانید و فعالیت ورودی و خروجی را ببینید. ۳. با تغییرات دستی کوچک و هد‌روم کافی شروع کنید.

۴. یکی از حالت‌های INPUT-AWARE LEARN، REFERENCE GUIDANCE یا DESCRIPTION GUIDANCE را انتخاب کنید. ۵. هنگام پخش، Learn پارامتر، باند/ماژول یا LEARN ALL را بزنید. ۶. با PREVIEW آزمایش و سپس APPLY یا REJECT کنید؛ فقط در صورت نیاز DETAILS / LOG را باز کنید. هم‌سطح مقایسه و پس از تصمیم ذخیره کنید.

### Insert در برابر برگشت کمکی

در Insert، Mix نسبت خشک و پردازش‌شده را پیش از Output تعیین می‌کند. روی برگشت ریورب یا دیلی، Mix را صددرصد و شدت افکت را با Send/Return میزبان تنظیم کنید. DEQ و DGate کنترل Mix کلی ندارند. DFX ترکیب محلی و اصلی دارد.

رابط فعلی کلید Power عمومی ندارد؛ برای مقایسهٔ کل نمونه از بای‌پس میزبان استفاده کنید. Mix صفر الزاماً بای‌پس نیست: Output می‌تواند فعال بماند و بافر افکت زمانی کار کند. OFF ماژول در DFX تضمین پاک‌شدن بافر نیست.

### نمونهٔ ترتیب پردازش

یک زنجیرهٔ شروع عملی: ویرایش، DGate در صورت نیاز، DEQ اصلاحی، DComp، DSess در صورت نیاز، DSat ملایم و فضا روی Send. این پیشنهاد تولید است، نه ترتیب داخلی ثابت باندل. جای مراحل را بر حسب منبع عوض و از روی‌هم‌گذاشتن تقویت‌ها بدون کنترل قله پرهیز کنید.

## رابط، کنترل‌ها و مترها

### ناب‌ها و انتخاب‌ها

برای تنظیم ناب، آن را بکشید و برای پیش‌فرض اعلام‌شده دوبار کلیک کنید. در کادر مقدار، عدد پشتیبانی‌شده وارد کنید؛ مقادیر به دامنه محدود می‌شوند. ورود فرکانس از Hz و kHz پشتیبانی می‌کند، ولی قالب نوشتاری فهرست اتومیشن میزبان ممکن است متفاوت باشد.

حالت ریورب، شکل فیلتر، کاراکتر سچوریشن، Stereo/Mono و OFF/ON ماژول‌های DFX انتخاب گسسته‌اند و به پارامتر میزبان متصل‌اند، نه دکمهٔ تزئینی. تغییر حالت الزاماً کراس‌فید بدون کلیک ندارد؛ سطح را کم و گذار را آزمایش کنید.

### پنل یادگیری و میز کار

منوی حالت، روش ساخت هدف را تعیین می‌کند. LOAD FILE مرجع محلی را انتخاب و DESCRIBE THE SOUND واژه‌های انگلیسی پشتیبانی‌شده را می‌گیرد. DETAILS / LOG گزارش اختیاری را باز می‌کند و پس از Learn خودکار باز نمی‌شود؛ BACK TO CONTROLS آن را می‌بندد. ADVANCED و MAIN CONTROLS صفحهٔ کنترل را بدون تغییر مقدار عوض می‌کنند. PREVIEW، APPLY، REJECT و UNDO چرخهٔ پیشنهاد را کنترل می‌کنند، نه تاریخچهٔ کامل جلسه.

### معنای نمایشگرها

شکل‌موج مرجع و نماهای تحلیل در رابط بومی از دادهٔ واقعی موجود استفاده می‌کنند. نمایش فرکانسی پنجرهٔ ۲۰۴۸ نمونه‌ای و ۱۲۸ نقطه دارد. تاریخچهٔ طیفی DVerb توالی طیف‌هاست، نه نمودار زمان میرایی اندازه‌گیری‌شده. بردارسنج DStereo نمونه‌های واقعی دو کانال را رسم می‌کند. نبود صوت می‌تواند نمایش را خالی یا بی‌حرکت کند.

DEQ پاسخ حاصل از پارامترهای فیلتر را رسم می‌کند. DComp، DGate و DSat منحنی انتقال توضیحی دارند؛ این رسم اندازه‌گیری کالیبرهٔ همهٔ حالت‌های پویا نیست. خوانش ورودی و خروجی، قلهٔ نمونه‌ای به سبک dBFS است، نه LUFS، لادنس RMS یا True Peak بین‌نمونه‌ای. GR نیز بالستیک تأییدشدهٔ متر ندارد.

### پنجره و تصاویر

رابط سخت‌افزاری با حفظ نسبت ابعاد بزرگ و کوچک می‌شود. راهنمای شناور، بسیاری از کنترل‌ها را معرفی می‌کند. تصاویر این دفترچه طرح گرافیکی‌اند، نه اسکرین‌شات بومی؛ نمودار و متر نمونهٔ آن‌ها نتیجهٔ تحلیل یا ادعای عملکرد نیست.

## حالت‌ها، دامنه‌ها و تأیید یادگیری

### یادگیری آگاه از ورودی

موتور، سطح، ضریب قله، انرژی باندهای کلی و ویژگی استریو را اندازه و مقدار محدودشده پیشنهاد می‌کند. فقط ۳۳ کنترل صوت‌محور Learn دارند؛ ۵۹ کنترل دیگر شامل Mix، تریم خروجی، انتخاب زمان و مسیر و فرکانس/Q/شکل EQ دستی می‌مانند. LEARN ALL و یادگیری باند یا ماژول نیز این ۵۹ کنترل را حفظ می‌کنند. شبکهٔ عصبی آموزش‌دیده یا استنتاج مدل ارائه نشده است. داشتن Learn به معنی استخراج یکتای پارامتر از هر صوت نیست.

### هدایت با مرجع

موفقیت حالت مرجع به مرجع معتبر نیاز دارد. در این نسخه، مرجع بر گین نرمال‌شدهٔ باندهای DEQ، پارامتر Width در DVerb و DDelay و DStereo و سطح Makeup در DComp اثر دارد. سایر پیشنهادها همچنان سیاست منبع یا هدف هستند. حلقهٔ مقایسهٔ خروجی رندرشده یا بازسازی مرجع دلخواه وجود ندارد.

### هدایت با توصیف

واژه‌های انگلیسی dark، warm، wide، smooth و cinematic را وارد کنید. این‌ها هدف رنگ، استریو، پوش یا فضا می‌دهند و همچنان ورودی صوتی لازم است. تحلیلگر فعلی نثر فارسی را تفسیر نمی‌کند. متن کاملاً ناشناخته خطا می‌دهد و در متن ترکیبی ممکن است فقط واژهٔ شناخته‌شده استفاده شود.

واژه‌های رایج نفی رد می‌شوند؛ not dark خطا می‌دهد و dark را فعال نمی‌کند. فقط واژهٔ کامل تطبیق دارد؛ darkness و smoothie به dark و smooth تبدیل نمی‌شوند. از دستور پیچیده، وعدهٔ شبیه‌سازی دقیق و صفت پشتیبانی‌نشده پرهیز کنید. عبارت dark, warm, wide, smooth, cinematic معتبر است، ولی نتیجه تخمین قابل‌شنیدن است، نه تضمین.

### کوچک‌ترین دامنهٔ مفید را انتخاب کنید

LEARN زیر یک پارامتر فقط همان شناسه را پیشنهاد می‌دهد. عمل باند یا ماژول، بخش اعلام‌شده را پوشش می‌دهد. عمل مستقل هر باند EQ فقط گین همان باند را یاد می‌گیرد و فرکانس، Q و شکل دستی می‌مانند. LEARN ALL فقط کنترل‌های واجد شرایط را جمع می‌کند؛ بهینه‌ساز مشترک نیست و پیشنهاد مستقل مجوز تغییر پنهان بخش دیگر نیست.

### پیش‌شنوی، اعمال، رد و بازگردانی

پیشنهاد تا عمل صریح پارامترها را عوض نمی‌کند. PREVIEW موقتاً مقدارها را برای شنیدن می‌نویسد؛ APPLY نگه می‌دارد و REJECT مقادیر پیش از پیش‌شنوی را بازمی‌گرداند. UNDO یک عمل یادگیریِ اعمال‌شده را تا پیش از ویرایش دستی/میزبان برمی‌گرداند؛ ویرایش جدید این بازگردانی را منقضی می‌کند و تاریخچهٔ چندسطحی وجود ندارد. بستن رابط در پیاده‌سازی فعلی پیش‌شنوی تأییدنشده را بازمی‌گرداند.

ویرایش دستی یا میزبان، پیشنهاد قبلی و Undo یادگیری را منقضی می‌کند؛ هنگام Preview، کنترل‌های دست‌نخورده بازمی‌گردند و ویرایش واقعی حفظ می‌شود. تا اعتبارسنجی کامل این تعامل، هنگام تحلیل پارامترها را اتومیشن یا ویرایش نکنید. پس از تغییر منبع دوباره Learn کنید. اطمینان، نمرهٔ ابتکاری است؛ احتمال کالیبره یا درصد کیفیت صوت نیست.

## فایل مرجع و روند دریافت صوت

### بارگذاری و اعتبارسنجی

LOAD FILE و یک WAV، AIFF/AIF یا FLAC قابل‌خواندن با رمزگشاهای پایهٔ JUCE را انتخاب کنید. انتخابگر همین پسوندها را فیلتر می‌کند؛ همهٔ گونه‌های کدک تضمین نیستند. رمزگشایی محلی و در Worker است. فقط ۳۰ ثانیهٔ نخست تحلیل می‌شود؛ بخش مرتبط مرجع را در ابتدای فایل بگذارید.

خواننده نرخ ۸ تا ۳۸۴ کیلوهرتز و حداقل ۱۰۲۴ نمونه می‌پذیرد. ویژگی ساکت یا نامعتبر رد می‌شود. مرجع مونو برای بافر تحلیل استریو تکرار می‌شود. نگاشت ویژهٔ فراگیر و داون‌میکس ادراکی ارائه نشده است.

### دریافت منبع

هنگام درخواست دامنه، میزبان باید صوت غیرساکت عبور دهد. دریافت ۶۵٬۵۳۶ نمونهٔ ورودی است: حدود ۱٫۴۹ ثانیه در ۴۴٫۱، ۱٫۳۷ ثانیه در ۴۸ و ۰٫۶۸ ثانیه در ۹۶ کیلوهرتز؛ نه کل آهنگ. با توقف پخش یا تعلیق ترک، ممکن است پس از ۱۵ ثانیه زمان تمام شود.

عبارتی نماینده با ترنزینت و کشش انتخاب کنید، نه فیداین یا قلهٔ غیرعادیِ منفرد. تحلیل از ورودی است، نه خروجی پردازش‌شده. پیشنهاد تازه مقدار را جایگزین می‌کند و الزاماً اصلاح افزایشی صدای فعلی شما نیست.

### لغو و خطا

REJECT یا لغو نتیجهٔ معلق را نامعتبر می‌کند؛ Worker محدود ممکن است در پس‌زمینه تمام شود. نتیجهٔ قدیمی را تأیید تازه فرض نکنید. اگر Capture busy پس از توقف باقی ماند، صوت را پخش کنید تا دریافت قبلی تمام و کنار گذاشته شود؛ سپس تحلیل تازه بخواهید.

Ready یعنی آماده، Analyzing یعنی دریافت یا تحلیل، Ready to audition یعنی وجود پیشنهاد، Previewing یعنی شنیدن موقت، Applied یعنی نگه‌داشتن و Cancelled یعنی کنارگذاری یا بازگردانی. Failed یا Reference unavailable موفقیت نیست. از تصویر سخت‌افزاری، نصب مدل عصبی را نتیجه نگیرید.

## پریست کارخانه و تمپوی پروژه

### ۵۰ برنامه برای هر پلاگین

هر محصول Init و ۴۹ گونهٔ پروفایل موسیقایی دارد؛ مجموعاً ۵۰۰ وضعیت کامل. از مرورگر بالای رابط، پیکان‌ها یا فهرست برنامهٔ میزبان انتخاب کنید. بارگذاری همهٔ تنظیم‌ها از جمله Mix و Output را تغییر می‌دهد؛ ابتدا مانیتور را کم کنید. ضبط، تحلیل معلق، پیش‌شنوی و بازگردانی یادگیری قبلی لغو می‌شوند؛ تنظیم قدیمی روی صدای تازه بازنمی‌گردد.

ستاره، تغییر نسبت به وضعیت کارخانه را نشان می‌دهد. برای حفظ مقدار ویرایش‌شده و شمارهٔ انتخاب، پروژهٔ DAW را ذخیره کنید. نام کارخانه فقط خواندنی است. فهرست هر محصول همهٔ نام‌ها و فایل Design/Presets/FactoryPresets.json مقدار همهٔ پارامترها را دارد. دستورهای عملیِ سایر صفحات صرفاً پیشنهادند، نه برنامهٔ نصب‌شدهٔ اضافه.

### Sync دیلی و تقسیم نت

SYNC را فعال و Division را انتخاب کنید. زمان میلی‌ثانیهٔ نت سیاه برابر ۶۰٬۰۰۰ تقسیم بر BPM میزبان است. D نت را ۱٫۵ برابر و T دوسوم می‌کند. انتخاب‌ها: 1/32، 1/32 D، 1/32 T، 1/16، 1/16 D، 1/16 T، 1/8، 1/8 D، 1/8 T، 1/4، 1/4 D، 1/4 T، 1/2، 1/2 D و 1/1.

HOST یعنی تمپوی معتبر میزبان؛ HOLD در نبود آن آخرین BPM معتبر را نگه می‌دارد و FALLBACK پیش از دریافت تمپوی معتبر از ۱۲۰ استفاده می‌کند. بازهٔ پشتیبانی ۱۰ تا ۴۰۰ است. Standalone تمپوی پروژه ندارد. Sync زمان آزاد، مدولاسیون و اختلاف استریو را غیرفعال می‌کند. تشخیص تمپو از صوت، Tap Tempo یا شروع مجدد بر اساس موقعیت آهنگ وجود ندارد.

### سازگاری و شنیدن

وضعیت قدیمی DDelay بدون شناسه‌های جدید از FREE MS شروع می‌کند. DVerb و DFX اکنون پخش آل‌پسِ ورودی، شکل‌دهی بم و میانی فقط در ورودی و شبکهٔ متعامد بدون اشباع دائمی دارند. مقدارهای ذخیره‌شده حفظ می‌شوند، ولی الگوریتم جدید صدا را تغییر می‌دهد؛ برای بازخوانی دقیق پروژهٔ قدیمی باینری قبلی را نگه دارید. نمونهٔ شنیداری، سیگنال مصنوعیِ اصلی است و تأیید کیفیت موسیقایی محسوب نمی‌شود.

## DVerb — ریورب و طراحی فضا

DVerb با یک شبکهٔ الگوریتمی هشت‌خطی، به صدا فضا می‌دهد. پیش‌تأخیر، شروع ریورب را از صدای مستقیم جدا می‌کند و میرایی، پخش‌شدگی و مدولاسیون، برگشت افکت را شکل می‌دهند. این پلاگین ابزار طراحی فضاست؛ پخش‌کنندهٔ پاسخ ضربه یا بازسازی دقیق یک اتاق نمونه‌برداری‌شده نیست.

Input → Pre-delay → Damped / diffused FDN → Stereo width → Dry/wet mix

### مرجع کنترل‌ها

#### Pre-delay / `predelay`

Range: 0 ms to 200 ms | Default: 20 ms

زمان پیش از تحریک ریورب است. برای جداکردن حملهٔ خشک از فضا افزایش دهید؛ مسیر خشک تأخیر نمی‌گیرد.

Learn: موجود

#### Size / `size`

Range: 0% to 100% | Default: 60%

طول تأخیرهای شبکه را تغییر می‌دهد. مقدار بیشتر، فاصلهٔ بازتاب‌ها و مقیاس ادراکی را عوض می‌کند؛ واحد آن متر نیست.

فقط دستی؛ بدون Learn

#### Decay / `decay`

Range: 0.1 s to 12 s | Default: 2 s

طرح میرایی بازخورد را کنترل می‌کند. مقدار بیشتر فضا را می‌کشد، ولی میرایی واقعی به میرایی طیفی، پخش‌شدگی، اندازه و حالت هم وابسته است.

Learn: موجود

#### Tone / `tone`

Range: 500 Hz to 18000 Hz | Default: 10000 Hz

فرکانس قطع میرایی پایین‌گذر شبکه است. مقدار کمتر، برگشت را تیره‌تر می‌کند؛ این کنترل فرکانسی است، نه شیب دسی‌بلی رنگ.

Learn: موجود

#### Mix / `mix`

Range: 0% to 100% | Default: 25%

ترکیب خطی صدای اصلی و ریورب است. برای برگشت کمکی کاملاً افکتی، صددرصد انتخاب کنید.

فقط دستی؛ بدون Learn

#### Width / `width`

Range: 0% to 200% | Default: 100%

مؤلفهٔ جانبی ریورب Wet را تغییر می‌دهد: صفر مونو، صددرصد عادی و دویست‌درصد دو برابر Side؛ بخش خشک را باز نمی‌کند.

Learn: موجود

#### Density / `density`

Range: 0% to 100% | Default: 70%

تحریک خطوط اضافهٔ شبکه را تغییر می‌دهد. مقدار کمتر مسیرهای اولیهٔ کمتری و مقدار بیشتر شبکهٔ اولیهٔ متراکم‌تری را تغذیه می‌کند.

فقط دستی؛ بدون Learn

#### Diffusion / `diffusion`

Range: 0% to 100% | Default: 70%

ضریب بازخورد چهار آل‌پسِ سری ورودی را کنترل می‌کند. مقدار بیشتر، حمله را پیش از شبکهٔ همواره متعامد پخش می‌کند؛ دیگر ماتریس شبکه را با مسیر مستقیم ترکیب نمی‌کند.

فقط دستی؛ بدون Learn

#### Modulation / `modulation`

Range: 0% to 100% | Default: 10%

عمق حرکت آهستهٔ طول تأخیرهاست. حرکت و بافت زیر و بمی می‌دهد؛ کنترل مستقل نرخ وجود ندارد.

فقط دستی؛ بدون Learn

#### Low cut / `lowcut`

Range: 20 Hz to 1000 Hz | Default: 120 Hz

بالاگذرِ تحریک پیش از شبکه است، نه فیلتری تکرارشونده در هر گردش. برای کاهش انباشت بم افزایش دهید.

Learn: موجود

#### Mid cut / `midcut`

Range: -12 dB to 0 dB | Default: -1 dB

کاهش پهن و ثابت میانی، یک‌بار بر تحریک اعمال می‌شود؛ دنباله را در هر گردش توخالی نمی‌کند و باند پارامتریک با فرکانس آزاد نیست.

فقط دستی؛ بدون Learn

#### Room/Hall/Plate/Shimmer / `mode`

Range: Room / Hall / Plate / Shimmer | Default: Hall

Room، Hall و Plate مقیاس تأخیر متفاوت دارند؛ Shimmer بازخورد گرِینی یک اکتاو بالاتر می‌افزاید. این یک انتخاب حالت است، نه شدت افکت.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Air / Focused
03. Vocal Air / Soft
04. Vocal Air / Open
05. Vocal Air / Parallel
06. Drum Room / Natural
07. Drum Room / Focused
08. Drum Room / Soft
09. Drum Room / Open
10. Drum Room / Parallel
11. Snare Plate / Natural
12. Snare Plate / Focused
13. Snare Plate / Soft
14. Snare Plate / Open
15. Snare Plate / Parallel
16. Piano Chamber / Natural
17. Piano Chamber / Focused
18. Piano Chamber / Soft
19. Piano Chamber / Open
20. Piano Chamber / Parallel
21. Acoustic Space / Natural
22. Acoustic Space / Focused
23. Acoustic Space / Soft
24. Acoustic Space / Open
25. Acoustic Space / Parallel
26. String Hall / Natural
27. String Hall / Focused
28. String Hall / Soft
29. String Hall / Open
30. String Hall / Parallel
31. Synth Cloud / Natural
32. Synth Cloud / Focused
33. Synth Cloud / Soft
34. Synth Cloud / Open
35. Synth Cloud / Parallel
36. Bright Shimmer / Natural
37. Bright Shimmer / Focused
38. Bright Shimmer / Soft
39. Bright Shimmer / Open
40. Bright Shimmer / Parallel
41. Dark Cathedral / Natural
42. Dark Cathedral / Focused
43. Dark Cathedral / Soft
44. Dark Cathedral / Open
45. Dark Cathedral / Parallel
46. Short Ambience / Natural
47. Short Ambience / Focused
48. Short Ambience / Soft
49. Short Ambience / Open
50. Short Ambience / Parallel

### استفادهٔ عملی

1. برای ارسال وکال، از Hall، پیش‌تأخیر ۲۰ تا ۴۰ میلی‌ثانیه و Decay حدود ۱٫۵ تا ۲٫۵ ثانیه شروع کنید.
2. در مسیر برگشت کمکی، Mix را روی ۱۰۰٪ بگذارید و شدت افکت را با سطح Send میزبان تنظیم کنید.
3. Low Cut حدود ۱۲۰ تا ۲۰۰ هرتز را امتحان کنید؛ اگر افکت وضوح حروف یا سنج را می‌پوشاند، Tone را کاهش دهید.
4. برای شکل‌دادن به انباشت فضا، Density و Diffusion را تنظیم کنید. پیش از شیمِر، مقدار کمی Modulation را امتحان کنید.
5. پس از قطع منبع، دنباله را بشنوید؛ اگر جمله‌ها روی هم محو می‌شوند، Decay یا سطح ارسال را کم کنید.

Learn فقط برای Pre-delay، Decay، Tone، Width و Low Cut موجود است. ضریب قله و طیف کلی نقطهٔ شروع می‌دهند و Width می‌تواند از مرجع کمک بگیرد. Size، Mix، حالت، Density، Diffusion، Modulation و Mid Cut دستی می‌مانند. RT60 مرجع یا پاسخ ضربهٔ اتاق استخراج نمی‌شود.

Decay یک کنترل طراحی است، نه اندازه‌گیری کالیبرهٔ RT60. حالت‌های Room، Hall و Plate زمان‌بندی شبکه را تغییر می‌دهند و نمایندهٔ یک سالن یا سخت‌افزار مشخص نیستند. شیمِر از بازخورد تغییر زیر و بمی دوگرِینی استفاده می‌کند؛ آرتیفکت‌های آن را با گوش بررسی کنید.

برای تغییر محدودتر، از دامنهٔ Space یا Colour استفاده کنید. Width بیش از ۱۰۰٪ مؤلفهٔ جانبی برگشت افکت را افزایش می‌دهد؛ مونو را کنترل کنید و ریورب بزرگ را فقط در حالت سولو قضاوت نکنید.

## DDelay — دیلی استریو

DDelay حالت FREE MS و حالت SYNC با ۱۵ تقسیم نت معمولی، نقطه‌دار و تریوله دارد. حالت آزاد، اختلاف کوچک زمان استریو و مدولاسیون را حفظ می‌کند؛ در Sync هر دو خط، طول نت یکسان دارند و مدولاسیون غیرفعال است. Filter و Width رنگ و پهنای تکرار را شکل می‌دهند.

Input → Stereo delay ↔ Filtered feedback → Wet width → Mix → Output

### مرجع کنترل‌ها

#### Time / `time`

Range: 1 ms to 2000 ms | Default: 375 ms

زمان تأخیر در FREE MS است و خط راست ۱٫۱٪ اختلاف دارد. در SYNC نادیده و غیرفعال می‌شود؛ برچسب تمپو زمان واقعی نت را نشان می‌دهد.

فقط دستی؛ بدون Learn

#### Feedback / `feedback`

Range: 0 (0%) to 0.95 (95%) | Default: 0.35 (35%)

سهم برگشتی به حلقه است. صفر تکرار بازگشتی ندارد و ۰٫۹۵ یعنی ۹۵٪؛ تعداد معین تکرار را تضمین نمی‌کند.

فقط دستی؛ بدون Learn

#### Filter / `filter`

Range: 200 Hz to 18000 Hz | Default: 6000 Hz

فرکانس قطع پایین‌گذرِ محتوای تأخیری و بازخورد است؛ مقدار کمتر تکرارها را تیره‌تر می‌کند.

Learn: موجود

#### Ping-pong / `pingpong`

Range: Stereo / Ping-pong | Default: Ping-pong

Stereo بازخورد را در همان کانال نگه می‌دارد؛ Ping-pong آن را به کانال مقابل می‌فرستد. ورودی همچنان به هر دو خط می‌رسد.

فقط دستی؛ بدون Learn

#### Modulation / `modulation`

Range: 0% to 100% | Default: 10%

عمق حرکت آهستهٔ زمان و زیر و بمی فقط در FREE MS است. برای حفظ زمان دقیق شبکه، در SYNC غیرفعال است.

فقط دستی؛ بدون Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 25%

ترکیب خطی خشک و Wet پیش از گین Output است. در صفر افکت شنیده نمی‌شود، ولی حافظهٔ دیلی پاک نمی‌شود.

فقط دستی؛ بدون Learn

#### Width / `width`

Range: 0% to 200% | Default: 100%

مقیاس مؤلفهٔ جانبی اکوی Wet است. پس از بازکردن، جمع مونو را بررسی کنید.

Learn: موجود

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

سطح دسی‌بلی پس از Mix است. اگر مجموع ورودی و تکرارها قله را بالا برد، کاهش دهید.

فقط دستی؛ بدون Learn

#### Tempo sync / `sync`

Range: FREE MS / SYNC | Default: FREE MS

FREE MS از Time و SYNC از BPM میزبان و Division استفاده می‌کند. با نبود تمپو، مقدار معتبر قبلی یا در ابتدا ۱۲۰ نگه داشته می‌شود. این انتخاب فقط دستی است.

فقط دستی؛ بدون Learn

#### Note division / `division`

Range: 15 notes: 1/32 to 1/1; D / T | Default: 1/8

طول نت در SYNC است: D نقطه‌دار با ضریب ۱٫۵ و T تریوله با ضریب دوسوم است. نت‌های معمولی از 1/32 تا 1/1 هستند؛ ۱۵ انتخاب در فصل پریست و تمپو آمده‌اند.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Eighth / Focused
03. Vocal Eighth / Soft
04. Vocal Eighth / Open
05. Vocal Eighth / Parallel
06. Dotted Guitar / Natural
07. Dotted Guitar / Focused
08. Dotted Guitar / Soft
09. Dotted Guitar / Open
10. Dotted Guitar / Parallel
11. Quarter Echo / Natural
12. Quarter Echo / Focused
13. Quarter Echo / Soft
14. Quarter Echo / Open
15. Quarter Echo / Parallel
16. Triplet Motion / Natural
17. Triplet Motion / Focused
18. Triplet Motion / Soft
19. Triplet Motion / Open
20. Triplet Motion / Parallel
21. Sixteenth Pulse / Natural
22. Sixteenth Pulse / Focused
23. Sixteenth Pulse / Soft
24. Sixteenth Pulse / Open
25. Sixteenth Pulse / Parallel
26. Stereo Half / Natural
27. Stereo Half / Focused
28. Stereo Half / Soft
29. Stereo Half / Open
30. Stereo Half / Parallel
31. Rock Slap / Natural
32. Rock Slap / Focused
33. Rock Slap / Soft
34. Rock Slap / Open
35. Rock Slap / Parallel
36. Tape Shadow / Natural
37. Tape Shadow / Focused
38. Tape Shadow / Soft
39. Tape Shadow / Open
40. Tape Shadow / Parallel
41. Wide Doubler / Natural
42. Wide Doubler / Focused
43. Wide Doubler / Soft
44. Wide Doubler / Open
45. Wide Doubler / Parallel
46. Ambient Cascade / Natural
47. Ambient Cascade / Focused
48. Ambient Cascade / Soft
49. Ambient Cascade / Open
50. Ambient Cascade / Parallel

### استفادهٔ عملی

1. SYNC را فعال و برای نت سیاه 1/4 را انتخاب کنید؛ در تمپوی ۱۲۰ میزبان، زمان نمایش‌داده‌شده ۵۰۰ میلی‌ثانیه است.
2. در تمپوی ۱۲۰، گزینهٔ 1/8 برابر ۲۵۰، 1/8 D نقطه‌دار برابر ۳۷۵ و 1/8 T تریوله حدود ۱۶۶٫۷ میلی‌ثانیه است. برای زمان دستی از FREE MS استفاده کنید.
3. در مسیر Insert، از Feedback حدود ۲۰ تا ۳۵٪ و Mix حدود ۱۰ تا ۲۵٪ شروع کنید. در رابط فعلی، Feedback ممکن است به شکل ضریب اعشاری نمایش داده شود.
4. برای تکرار تیره‌تر، Filter را پایین بیاورید؛ اگر با تنظیم سازگار است، Ping-pong را برای بازخورد متناوب فعال کنید.
5. برای زیر و بمی پایدار، Modulation را کم نگه دارید و پیش از پایان محدودهٔ رندر، برای محوشدن تکرار آخر زمان بگذارید.

فقط Filter و Width دارای Learn هستند. Time، Sync، Division، Feedback، Modulation، مسیر، Mix و Output در همهٔ دامنه‌ها دستی می‌مانند. Sync تمپوی پروژه را می‌خواند؛ یادگیری از صوت تمپو استخراج نمی‌کند.

SYNC از BPM میزبان در بازهٔ ۱۰ تا ۴۰۰ استفاده می‌کند. رابط HOST، نگه‌داری آخرین تمپو با HOLD یا پیش‌فرض ۱۲۰ با FALLBACK را نشان می‌دهد. نت گرد در تمپوی ۱۰، برابر ۲۴ ثانیه است. تغییر تمپو یا تقسیم نت می‌تواند گذار زیر و بمی بسازد. Tap Tempo یا تشخیص BPM صوت وجود ندارد.

Stereo بازخورد را در همان کانال نگه می‌دارد و Ping-pong آن را متقاطع می‌کند. ورودی وارد هر دو کانال می‌شود؛ پس منبع مونو الزاماً نخستین تکرار کاملاً چپ یا راست ندارد. Mix صفر فقط برگشت افکت را حذف می‌کند، نه محتوای بافر را.

## DComp — کمپرسور داینامیک

DComp با آشکارسازی پوش قله، نسبت، حمله، رهایی و زانوی نرم، سطح بالاتر از آستانه را کاهش می‌دهد. Makeup داخل مسیر پردازش‌شده و Output پس از ترکیب موازی عمل می‌کند. آشکارساز، متر لادنس نیست و این پردازنده لیمیتر دیوارآجری محسوب نمی‌شود.

Input → Peak envelope / gain curve → Makeup → Parallel mix → Output

### مرجع کنترل‌ها

#### Threshold / `threshold`

Range: -60 dB to 0 dB | Default: -18 dB

سطح آشکارساز برای آغاز کمپرس است که Knee آن را تعدیل می‌کند. مقدار پایین‌تر معمولاً کاهش بیشتری می‌دهد.

Learn: موجود

#### Ratio / `ratio`

Range: 1:1 to 20:1 | Default: 4:1

نسبت کمپرس بالای زانوست. در منحنی ثابتِ ۴ به ۱، افزایش ۴ دسی‌بلی آشکارساز تقریباً به ۱ دسی‌بل تبدیل می‌شود.

Learn: موجود

#### Attack / `attack`

Range: 0.1 ms to 200 ms | Default: 15 ms

زمان پاسخ پوش به افزایش سطح است. کوتاه‌تر حمله را سریع‌تر می‌گیرد و بلندتر می‌تواند ضربهٔ اولیه را حفظ کند.

Learn: موجود

#### Release / `release`

Range: 10 ms to 2000 ms | Default: 150 ms

زمان بازیابی پوش با افت سطح است. خیلی کوتاه ممکن است پمپاژ و خیلی بلند سرکوب جملهٔ بعدی ایجاد کند.

Learn: موجود

#### Knee / `knee`

Range: 0 dB to 24 dB | Default: 6 dB

پهنای گذار پیرامون آستانه است. صفر زانوی سخت و مقدار بزرگ‌تر شروع نرم‌تر می‌دهد.

فقط دستی؛ بدون Learn

#### Makeup / `makeup`

Range: 0 dB to 24 dB | Default: 0 dB

گین شاخهٔ کمپرس‌شده پیش از Mix است. دستی است و سامانهٔ جبران خودکار لادنس نیست.

Learn: موجود

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

شاخهٔ اصلی را با شاخهٔ کمپرس و Makeup ترکیب می‌کند و آستانهٔ آشکارساز را تغییر نمی‌دهد.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین نهایی پس از ترکیب موازی است؛ برای هم‌سطح‌سازی و هد‌روم استفاده کنید.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Level / Focused
03. Vocal Level / Soft
04. Vocal Level / Open
05. Vocal Level / Parallel
06. Drum Punch / Natural
07. Drum Punch / Focused
08. Drum Punch / Soft
09. Drum Punch / Open
10. Drum Punch / Parallel
11. Bass Control / Natural
12. Bass Control / Focused
13. Bass Control / Soft
14. Bass Control / Open
15. Bass Control / Parallel
16. Acoustic Even / Natural
17. Acoustic Even / Focused
18. Acoustic Even / Soft
19. Acoustic Even / Open
20. Acoustic Even / Parallel
21. Piano Gentle / Natural
22. Piano Gentle / Focused
23. Piano Gentle / Soft
24. Piano Gentle / Open
25. Piano Gentle / Parallel
26. Bus Glue / Natural
27. Bus Glue / Focused
28. Bus Glue / Soft
29. Bus Glue / Open
30. Bus Glue / Parallel
31. Parallel Weight / Natural
32. Parallel Weight / Focused
33. Parallel Weight / Soft
34. Parallel Weight / Open
35. Parallel Weight / Parallel
36. Transient Catch / Natural
37. Transient Catch / Focused
38. Transient Catch / Soft
39. Transient Catch / Open
40. Transient Catch / Parallel
41. Soft Speech / Natural
42. Soft Speech / Focused
43. Soft Speech / Soft
44. Soft Speech / Open
45. Soft Speech / Parallel
46. Sustain Hold / Natural
47. Sustain Hold / Focused
48. Sustain Hold / Soft
49. Sustain Hold / Open
50. Sustain Hold / Parallel

### استفادهٔ عملی

1. برای نقطهٔ شروع وکال، Ratio را حدود ۳ به ۱، Attack را ۱۵ تا ۲۵ و Release را ۱۰۰ تا ۲۰۰ میلی‌ثانیه انتخاب کنید.
2. Threshold را پایین بیاورید تا جمله‌های عادی چند دسی‌بل کاهش بگیرند؛ بخش‌های آرام و بلند را بررسی کنید.
3. برای ورود نرم‌تر به کمپرس، Knee بزرگ‌تری انتخاب کنید.
4. Makeup را فقط برای مقایسهٔ هم‌سطح بالا ببرید و برای ترکیب موازی از Mix استفاده کنید.
5. اگر صدا پمپاژ می‌کند، پیش از افزایش نسبت یا Makeup، Release و Threshold را بازبینی کنید.

Threshold، Ratio، Attack، Release و Makeup دارای Learn هستند و Knee، Mix و Output دستی می‌مانند. سطح و ضریب قله داینامیک را هدایت و smooth زمان رهایی را بلندتر می‌کند. Makeup اختلاف RMS هدف و ورودی است، نه جبران اندازه‌گیری‌شدهٔ کاهش گین پردازش‌شده. بهینه‌سازی خروجی یا تطبیق خودکار لادنس وجود ندارد.

پوش‌های چپ و راست مستقل‌اند و کنترل Stereo Link وجود ندارد؛ ترنزینت‌های متفاوت ممکن است تصویر استریو را جابه‌جا کنند. خوانش GR، مقدار محاسباتی پیاده‌سازی است، نه متر تأییدشدهٔ بیشینهٔ کاهش استریو.

در نسبت ۱ به ۱ منحنی کمپرس خنثی است، ولی Makeup، Mix و Output همچنان می‌توانند سطح را تغییر دهند. Attack و Release زمان هموارسازی‌اند، نه Look-ahead یا Hold. نسخهٔ بلندتر را بدون هم‌سطح‌کردن بهتر فرض نکنید.

## DEQ — اکولایزر چهار باند

DEQ چهار باند فیلتر RBJ پشت‌سرهم دارد. فرکانس، گین، Q و شکل هر باند مستقل است و همان باند جداگانه Learn می‌شود. نام‌های Low، Low Mid، Presence و Air فقط چیدمان پیشنهادی‌اند؛ محدودیت ثابت فرکانسی نیستند و هر باند کل بازهٔ اعلام‌شده را پوشش می‌دهد.

Input → Band 1 → Band 2 → Band 3 → Band 4 → Output

### مرجع کنترل‌ها

#### Frequency / `b1freq`

Range: 20 Hz to 20000 Hz | Default: 100 Hz

فرکانس مرکزی Bell یا فرکانس گذار شلف است. هر باند کل دامنهٔ دستی را دارد و نام پنل محدودش نمی‌کند.

فقط دستی؛ بدون Learn

#### Gain / `b1gain`

Range: -18 dB to 18 dB | Default: 0 dB

تقویت یا کاهشِ شکل انتخاب‌شده است. صفر خنثی است. پیشنهاد Learn به مثبت یا منفی ۶ دسی‌بل محدودتر از دامنهٔ دستی است.

Learn: موجود

#### Q / `b1q`

Range: 0.2 to 8 | Default: 0.707

پهنای Bell است؛ Q بزرگ‌تر باریک‌تر است. شلف فعلی شیب ثابت دارد و از این مقدار Q استفاده نمی‌کند.

فقط دستی؛ بدون Learn

#### Bell/Low shelf/High shelf / `b1shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell، Low Shelf یا High Shelf است. یادگیری این انتخاب دستی را حفظ می‌کند؛ L-SHELF و H-SHELF به معنای شلف پایین و بالا هستند.

فقط دستی؛ بدون Learn

#### Frequency / `b2freq`

Range: 20 Hz to 20000 Hz | Default: 400 Hz

فرکانس مرکزی Bell یا فرکانس گذار شلف است. هر باند کل دامنهٔ دستی را دارد و نام پنل محدودش نمی‌کند.

فقط دستی؛ بدون Learn

#### Gain / `b2gain`

Range: -18 dB to 18 dB | Default: 0 dB

تقویت یا کاهشِ شکل انتخاب‌شده است. صفر خنثی است. پیشنهاد Learn به مثبت یا منفی ۶ دسی‌بل محدودتر از دامنهٔ دستی است.

Learn: موجود

#### Q / `b2q`

Range: 0.2 to 8 | Default: 0.707

پهنای Bell است؛ Q بزرگ‌تر باریک‌تر است. شلف فعلی شیب ثابت دارد و از این مقدار Q استفاده نمی‌کند.

فقط دستی؛ بدون Learn

#### Bell/Low shelf/High shelf / `b2shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell، Low Shelf یا High Shelf است. یادگیری این انتخاب دستی را حفظ می‌کند؛ L-SHELF و H-SHELF به معنای شلف پایین و بالا هستند.

فقط دستی؛ بدون Learn

#### Frequency / `b3freq`

Range: 20 Hz to 20000 Hz | Default: 2500 Hz

فرکانس مرکزی Bell یا فرکانس گذار شلف است. هر باند کل دامنهٔ دستی را دارد و نام پنل محدودش نمی‌کند.

فقط دستی؛ بدون Learn

#### Gain / `b3gain`

Range: -18 dB to 18 dB | Default: 0 dB

تقویت یا کاهشِ شکل انتخاب‌شده است. صفر خنثی است. پیشنهاد Learn به مثبت یا منفی ۶ دسی‌بل محدودتر از دامنهٔ دستی است.

Learn: موجود

#### Q / `b3q`

Range: 0.2 to 8 | Default: 0.707

پهنای Bell است؛ Q بزرگ‌تر باریک‌تر است. شلف فعلی شیب ثابت دارد و از این مقدار Q استفاده نمی‌کند.

فقط دستی؛ بدون Learn

#### Bell/Low shelf/High shelf / `b3shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell، Low Shelf یا High Shelf است. یادگیری این انتخاب دستی را حفظ می‌کند؛ L-SHELF و H-SHELF به معنای شلف پایین و بالا هستند.

فقط دستی؛ بدون Learn

#### Frequency / `b4freq`

Range: 20 Hz to 20000 Hz | Default: 10000 Hz

فرکانس مرکزی Bell یا فرکانس گذار شلف است. هر باند کل دامنهٔ دستی را دارد و نام پنل محدودش نمی‌کند.

فقط دستی؛ بدون Learn

#### Gain / `b4gain`

Range: -18 dB to 18 dB | Default: 0 dB

تقویت یا کاهشِ شکل انتخاب‌شده است. صفر خنثی است. پیشنهاد Learn به مثبت یا منفی ۶ دسی‌بل محدودتر از دامنهٔ دستی است.

Learn: موجود

#### Q / `b4q`

Range: 0.2 to 8 | Default: 0.707

پهنای Bell است؛ Q بزرگ‌تر باریک‌تر است. شلف فعلی شیب ثابت دارد و از این مقدار Q استفاده نمی‌کند.

فقط دستی؛ بدون Learn

#### Bell/Low shelf/High shelf / `b4shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell، Low Shelf یا High Shelf است. یادگیری این انتخاب دستی را حفظ می‌کند؛ L-SHELF و H-SHELF به معنای شلف پایین و بالا هستند.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Clarity / Focused
03. Vocal Clarity / Soft
04. Vocal Clarity / Open
05. Vocal Clarity / Parallel
06. Warm Voice / Natural
07. Warm Voice / Focused
08. Warm Voice / Soft
09. Warm Voice / Open
10. Warm Voice / Parallel
11. Drum Presence / Natural
12. Drum Presence / Focused
13. Drum Presence / Soft
14. Drum Presence / Open
15. Drum Presence / Parallel
16. Bass Definition / Natural
17. Bass Definition / Focused
18. Bass Definition / Soft
19. Bass Definition / Open
20. Bass Definition / Parallel
21. Acoustic Detail / Natural
22. Acoustic Detail / Focused
23. Acoustic Detail / Soft
24. Acoustic Detail / Open
25. Acoustic Detail / Parallel
26. Piano Body / Natural
27. Piano Body / Focused
28. Piano Body / Soft
29. Piano Body / Open
30. Piano Body / Parallel
31. String Silk / Natural
32. String Silk / Focused
33. String Silk / Soft
34. String Silk / Open
35. String Silk / Parallel
36. Synth Depth / Natural
37. Synth Depth / Focused
38. Synth Depth / Soft
39. Synth Depth / Open
40. Synth Depth / Parallel
41. Mix Polish / Natural
42. Mix Polish / Focused
43. Mix Polish / Soft
44. Mix Polish / Open
45. Mix Polish / Parallel
46. Dark Texture / Natural
47. Dark Texture / Focused
48. Dark Texture / Soft
49. Dark Texture / Open
50. Dark Texture / Parallel

### استفادهٔ عملی

1. از گین صفر همهٔ باندها شروع کنید و پیش از تقویت، مسئلهٔ طیفی را در میکس کامل پیدا کنید.
2. اگر صدا گِلی است، کاهش پهنِ ۱ تا ۳ دسی‌بل در لو‌مید با Q حدود ۰٫۷ تا ۱٫۲ را امتحان کنید.
3. برای مقایسه با مرجع، محتوای قابل‌مقایسه بارگذاری و حالت REFERENCE GUIDANCE را انتخاب کنید.
4. با LEARN BAND 2 یا Learn مربوط به GAIN فقط گین باند دوم پیشنهاد می‌گیرد؛ هر دو، فرکانس و Q و شکل را حفظ می‌کنند.
5. پاسخ را بررسی و با سطح برابر مقایسه کنید؛ تقویت زیاد ممکن است پردازندهٔ بعدی را اورلود کند.

فقط گین چهار باند Learn دارد و پیشنهاد به مثبت یا منفی ۶ دسی‌بل محدود است. انرژی نرمال‌شدهٔ باندهای کلی گین را هدایت می‌کند. Learn باند فقط گین همان باند و LEARN ALL هر چهار گین را پوشش می‌دهد. فرکانس، Q و شکل در تنظیم دستی شما می‌مانند و در ADVANCED هستند؛ صفحهٔ اصلی چهار گین دارد.

یادگیرنده از چهار باند کلی منبع و هدف استفاده می‌کند و رزونانس دقیق، ماسکینگ یا پاسخ معکوس اتاق را تشخیص نمی‌دهد. فرکانس، Q و شکل دستی‌اند و تمام دامنه‌های یادگیری آن‌ها را حفظ می‌کنند. Q فقط روی Bell اثر دارد و شیب شلف ثابت است.

در این بیلد، DEQ پارامتر Mix، Output یا بای‌پس مستقل باند ندارد. گین صفر، باند را خنثی می‌کند و برای کل نمونه از بای‌پس میزبان استفاده کنید. EQ از نوع فاز خطی نیست؛ در مسیر موازی، یکسان‌بودن فاز خشک و فیلترشده را فرض نکنید.

## DSat — سچوریشن هارمونیک

DSat یکی از سه تابع غیرخطی را اعمال می‌کند و با Density، صدای غیرخطی مستقیم را با نسخهٔ پایین‌گذرشدهٔ آن ترکیب می‌کند. Drive سطح ورودی طبقهٔ غیرخطی را تغییر می‌دهد؛ Tone و Density با هم روشنایی را تعیین می‌کنند. Output کنترل گین مستقل پس از Mix است.

Input → Drive / nonlinear character → Tone-density blend → Mix → Output

### مرجع کنترل‌ها

#### Drive / `drive`

Range: 0 dB to 30 dB | Default: 6 dB

گین ورودی تابع غیرخطی است. افزایش آن هارمونیک و سطح را با هم تغییر می‌دهد؛ تریم شفاف ورودی نیست.

Learn: موجود

#### Tone / `tone`

Range: 500 Hz to 18000 Hz | Default: 10000 Hz

فرکانس قطع پایین‌گذر شاخهٔ غیرخطی فیلترشده است. شنیده‌شدن اثر آن به Density وابسته است.

Learn: موجود

#### Soft/Hard/Asymmetric / `mode`

Range: Soft / Hard / Asymmetric | Default: Soft

Soft از tanh، Hard از کلیپ موج درایوشده و Asymmetric از tanh جابه‌جا‌شده با جبران آفست ثابت استفاده می‌کند.

فقط دستی؛ بدون Learn

#### Density / `density`

Range: 0% to 100% | Default: 50%

ترکیب به سمت شاخهٔ غیرخطی فیلترشده با Tone است؛ کنترل مستقل تعداد هارمونیک یا تراکم کمپرس نیست.

فقط دستی؛ بدون Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 50%

ترکیب موازی سیگنال اصلی و غیرخطی، پیش از Output است.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین پس از Mix است؛ جبران خودکار افزایش Drive اعمال نمی‌شود.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Warmth / Focused
03. Vocal Warmth / Soft
04. Vocal Warmth / Open
05. Vocal Warmth / Parallel
06. Drum Edge / Natural
07. Drum Edge / Focused
08. Drum Edge / Soft
09. Drum Edge / Open
10. Drum Edge / Parallel
11. Bass Grit / Natural
12. Bass Grit / Focused
13. Bass Grit / Soft
14. Bass Grit / Open
15. Bass Grit / Parallel
16. Acoustic Glow / Natural
17. Acoustic Glow / Focused
18. Acoustic Glow / Soft
19. Acoustic Glow / Open
20. Acoustic Glow / Parallel
21. Piano Felt / Natural
22. Piano Felt / Focused
23. Piano Felt / Soft
24. Piano Felt / Open
25. Piano Felt / Parallel
26. Bus Color / Natural
27. Bus Color / Focused
28. Bus Color / Soft
29. Bus Color / Open
30. Bus Color / Parallel
31. Synth Asymmetry / Natural
32. Synth Asymmetry / Focused
33. Synth Asymmetry / Soft
34. Synth Asymmetry / Open
35. Synth Asymmetry / Parallel
36. LoFi Shadow / Natural
37. LoFi Shadow / Focused
38. LoFi Shadow / Soft
39. LoFi Shadow / Open
40. LoFi Shadow / Parallel
41. Parallel Crunch / Natural
42. Parallel Crunch / Focused
43. Parallel Crunch / Soft
44. Parallel Crunch / Open
45. Parallel Crunch / Parallel
46. Smooth Harmonics / Natural
47. Smooth Harmonics / Focused
48. Smooth Harmonics / Soft
49. Smooth Harmonics / Open
50. Smooth Harmonics / Parallel

### استفادهٔ عملی

1. از Soft، Drive حدود ۳ تا ۶ دسی‌بل و Mix متوسط شروع کنید.
2. Drive را به‌تدریج بالا ببرید و هم‌زمان Output را برای مقایسهٔ منصفانه کاهش دهید.
3. برای تیره‌کردن جزء غیرخطی، Tone را همراه Density بیشتر از صفر به کار ببرید.
4. اگر بافت تیزتر به منبع کمک می‌کند، Hard را امتحان کنید؛ Asymmetric توازن متفاوتی در منحنی می‌دهد.
5. سنج‌ها و هارمونیک‌های بالا را دوباره بشنوید و برای نتیجهٔ کنترل‌شده‌تر از Mix استفاده کنید.

فقط Drive و Tone دارای Learn هستند و از قله، ضریب قله و انرژی باند بالا کمک می‌گیرند. حالت، Density، Mix و Output دستی می‌مانند. این سیستم تشخیص آموزش‌دیدهٔ تیپ/لامپ، تطبیق هارمونیک مرجع یا تطبیق خودکار لادنس نیست.

اورسمپلینگ پیاده‌سازی نشده است؛ Drive زیاد و منابع روشن می‌توانند علیاس ایجاد کنند. منحنی نامتقارن یک آفست ثابت را کم می‌کند، اما فیلتر عمومی حذف DC ندارد. جبران خودکار سطح خروجی نیز وجود ندارد.

در Density صفر، شاخهٔ فیلترشده حذف می‌شود؛ بنابراین Tone نقش عملی چندانی ندارد. در Density صددرصد، شاخهٔ غیرخطی Wet کاملاً پایین‌گذر می‌شود. کلیپ سخت، گزینهٔ طراحی صداست و لیمیتر ایمنی مسترینگ نیست.

## DTrans — طراح ترنزینت

DTrans پوش‌های دامنهٔ سریع و آهسته را مقایسه می‌کند تا شدت شروع ضربه را تخمین بزند. Attack و Sustain بر اساس آن گین اعمال می‌کنند و Sensitivity و Shape جداسازی را تغییر می‌دهند. برخلاف Attack کمپرسور، Attack این پلاگین گینِ دسی‌بلی است، نه زمان میلی‌ثانیه‌ای.

Input → Fast / slow envelope comparison → Attack-sustain gain → Mix → Output

### مرجع کنترل‌ها

#### Attack / `attack`

Range: -18 dB to 18 dB | Default: 0 dB

گینِ بخش‌های با شروع قوی است. مثبت حمله را برجسته و منفی آن را نرم می‌کند. واحد: دسی‌بل.

Learn: موجود

#### Sustain / `sustain`

Range: -18 dB to 18 dB | Default: 0 dB

گین با وزن بیشتر روی بخش غیرآغاز سیگنال است. مقدار مثبت علاوه بر کشش موسیقایی، اتاق و نویز را هم بالا می‌آورد.

فقط دستی؛ بدون Learn

#### Sensitivity / `sensitivity`

Range: 0% to 100% | Default: 50%

اختلاف نرمال‌شدهٔ پوش سریع و آهسته را مقیاس می‌کند؛ آستانهٔ دسی‌بلی یا کنترل زمان نیست.

Learn: موجود

#### Shape / `shape`

Range: 0.2 to 3 | Default: 1

توانِ وزن آغاز ضربه است. مقدار بالاتر اثر Attack را بیشتر روی شروع‌های قوی متمرکز می‌کند.

فقط دستی؛ بدون Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

ترکیب موازی سیگنال اصلی و شکل‌داده‌شده با پوش است.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین نهایی پس از افکت پوش و ترکیب موازی است.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Kick Punch / Focused
03. Kick Punch / Soft
04. Kick Punch / Open
05. Kick Punch / Parallel
06. Snare Snap / Natural
07. Snare Snap / Focused
08. Snare Snap / Soft
09. Snare Snap / Open
10. Snare Snap / Parallel
11. Room Tamer / Natural
12. Room Tamer / Focused
13. Room Tamer / Soft
14. Room Tamer / Open
15. Room Tamer / Parallel
16. Bass Pick / Natural
17. Bass Pick / Focused
18. Bass Pick / Soft
19. Bass Pick / Open
20. Bass Pick / Parallel
21. Acoustic Pluck / Natural
22. Acoustic Pluck / Focused
23. Acoustic Pluck / Soft
24. Acoustic Pluck / Open
25. Acoustic Pluck / Parallel
26. Soft Piano / Natural
27. Soft Piano / Focused
28. Soft Piano / Soft
29. Soft Piano / Open
30. Soft Piano / Parallel
31. Percussion Tight / Natural
32. Percussion Tight / Focused
33. Percussion Tight / Soft
34. Percussion Tight / Open
35. Percussion Tight / Parallel
36. Pad Bloom / Natural
37. Pad Bloom / Focused
38. Pad Bloom / Soft
39. Pad Bloom / Open
40. Pad Bloom / Parallel
41. Drum Sustain / Natural
42. Drum Sustain / Focused
43. Drum Sustain / Soft
44. Drum Sustain / Open
45. Drum Sustain / Parallel
46. Parallel Attack / Natural
47. Parallel Attack / Focused
48. Parallel Attack / Soft
49. Parallel Attack / Open
50. Parallel Attack / Parallel

### استفادهٔ عملی

1. برای درام جمع‌وجورتر، Attack مثبت ۲ تا ۴ و Sustain منفی ۲ تا ۴ دسی‌بل را امتحان کنید.
2. هنگام مقایسهٔ ضربه‌های کوتاه و کشیده، Sensitivity را نزدیک میانه نگه دارید.
3. اگر اثر انتخابی‌تری روی شروع‌های قوی می‌خواهید، Shape را افزایش دهید.
4. اگر پردازش از بدنهٔ ساز جدا به گوش می‌رسد، Mix را کم کنید.
5. ضربه‌های آرام و نشت سنج را بررسی و سپس Output را برای حفظ هد‌روم بعدی تنظیم کنید.

Learn با کمک ضریب قله فقط برای Attack و Sensitivity موجود است. Sustain، Shape، Mix و Output دستی می‌مانند. نتیجه برآورد نقطهٔ شروع محافظه‌کارانه است، نه بازسازی پوش مرجع یا تشخیص آموزش‌دیدهٔ نوع درام.

Sustain گینِ وزن‌دهی‌شده با پوش است؛ کنترل مستقل زمان میرایی یا جداساز منبع نیست. مقادیر مثبت زیاد می‌توانند نویز و صدای اتاق را برجسته کنند. پوش مستقل کانال‌ها ممکن است توازن استریو را تغییر دهد.

با تغییرات کوچک دسی‌بلی شروع کنید. روی منبع بدون شروع‌های مشخص، Sustain ممکن است غالب شود. نمایش پوش مرکزی، دامنهٔ سیگنال زنده را نشان می‌دهد و نمودار کالیبرهٔ ثابت زمانی یا اطمینان تشخیص ترنزینت نیست.

## DStereo — طراح تصویر استریو

DStereo مؤلفه‌های میانی و جانبی را از مجموع و تفاضل دو کانال می‌سازد، سطح آن‌ها را تغییر می‌دهد و استریو را بازسازی می‌کند. Depth نسخهٔ پایین‌گذرِ Side را ترکیب می‌کند؛ بنابراین روشنایی کناره‌ها را تغییر می‌دهد، نه فاصلهٔ فیزیکی یا تأخیر هاس را. بردارسنج و همبستگی به بررسی نتیجه کمک می‌کنند.

L/R → Mid/Side → Center / side / width / depth → Stereo or mono wet path → Mix → L/R output

### مرجع کنترل‌ها

#### Width / `width`

Range: 0% to 200% | Default: 100%

مؤلفهٔ Side مسیر Wet را ضرب می‌کند. صددرصد پیش از گین Side بهرهٔ واحد و دویست‌درصد دو برابر دامنهٔ Side است.

Learn: موجود

#### Depth / `depth`

Range: 0% to 100% | Default: 0%

Side را به نسخهٔ پایین‌گذرشده نزدیک می‌کند. مقدار بیشتر کناره‌ها را تیره می‌کند، نه اینکه منبع را در فضای اندازه‌گیری‌شده جابه‌جا کند.

فقط دستی؛ بدون Learn

#### Center / `center`

Range: -18 dB to 6 dB | Default: 0 dB

گین مؤلفهٔ Mid است؛ موقعیت پن یا الگوریتم استخراج مرکز نیست.

فقط دستی؛ بدون Learn

#### Side / `side`

Range: -18 dB to 6 dB | Default: 0 dB

گین مؤلفهٔ Side در کنار Width است. هر دو می‌توانند قله‌های جانبی را افزایش دهند.

Learn: موجود

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

استریوی اصلی را با شاخهٔ پردازش‌شدهٔ استریو یا مونو ترکیب می‌کند. زیر صددرصد، حتی با Mono روشن، استریوی خشک باقی می‌ماند.

فقط دستی؛ بدون Learn

#### Mono monitor / `mono`

Range: Stereo / Mono | Default: Stereo

Stereo بخش Side مسیر Wet را نگه می‌دارد و Mono آن را صفر می‌کند. برای شنیدن کاملاً مونو، Mix را صددرصد کنید.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Focus / Focused
03. Vocal Focus / Soft
04. Vocal Focus / Open
05. Vocal Focus / Parallel
06. Drum Stage / Natural
07. Drum Stage / Focused
08. Drum Stage / Soft
09. Drum Stage / Open
10. Drum Stage / Parallel
11. Bass Center / Natural
12. Bass Center / Focused
13. Bass Center / Soft
14. Bass Center / Open
15. Bass Center / Parallel
16. Acoustic Pair / Natural
17. Acoustic Pair / Focused
18. Acoustic Pair / Soft
19. Acoustic Pair / Open
20. Acoustic Pair / Parallel
21. Piano Stage / Natural
22. Piano Stage / Focused
23. Piano Stage / Soft
24. Piano Stage / Open
25. Piano Stage / Parallel
26. String Field / Natural
27. String Field / Focused
28. String Field / Soft
29. String Field / Open
30. String Field / Parallel
31. Synth Spread / Natural
32. Synth Spread / Focused
33. Synth Spread / Soft
34. Synth Spread / Open
35. Synth Spread / Parallel
36. Dark Sides / Natural
37. Dark Sides / Focused
38. Dark Sides / Soft
39. Dark Sides / Open
40. Dark Sides / Parallel
41. Narrow Room / Natural
42. Narrow Room / Focused
43. Narrow Room / Soft
44. Narrow Room / Open
45. Narrow Room / Parallel
46. Mix Perspective / Natural
47. Mix Perspective / Focused
48. Mix Perspective / Soft
49. Mix Perspective / Open
50. Mix Perspective / Parallel

### استفادهٔ عملی

1. از Width صددرصد، Center و Side صفر دسی‌بل و Depth صفر شروع کنید.
2. روی منبعی که از قبل استریو است، Width حدود ۱۱۰ تا ۱۲۵٪ را امتحان کنید و مرکز تصویر را بشنوید.
3. همبستگی را ببینید و در Mix صددرصد مونو را مقایسه کنید. همبستگی منفی هشدار است، نه امتیاز کامل کیفیت.
4. اگر کناره‌ها بیش از حد روشن‌اند، Depth ملایم به کار ببرید؛ اگر تمرکز از بین رفت، Width را بازگردانید.
5. با هدفون و اسپیکر بررسی کنید و جمع مونو در میزبان را هم به‌عنوان کنترل دوم بشنوید.

فقط Width و Side دارای Learn هستند. نسبت جانبی به RMS و همبستگی، Width را هدایت و نسبت مرجع می‌تواند هدف آن را تعیین کند. همبستگی منفی Width را باریک و Side را کم می‌کند. Depth، Center، Mix و Mono دستی می‌مانند و سازگاری مونو خروجی باید جدا بررسی شود.

کنترل Mono فقط مسیر Wet را مونو می‌کند. برای شنیدن کاملاً مونو در خود پلاگین، Mix را صددرصد کنید؛ در مقادیر کمتر، استریوی خشک باقی می‌ماند. ورودی مونو یا دو کانال یکسان مؤلفهٔ Side ندارد؛ Width نمی‌تواند از آن جزئیات استریو بسازد.

Width صفر، Side مسیر Wet را حذف می‌کند؛ صددرصد پیش از کنترل‌های دیگر بهرهٔ واحد است و دویست‌درصد آن را دو برابر می‌کند. گین Center و Side می‌توانند قله‌ها را بالا ببرند. بردارسنج نمونه‌های واقعی را نمایش می‌دهد؛ سکون آن ممکن است فقط نبود صوت تازه باشد.

## DSess — دی‌اسر وابسته به فرکانس

DSess یک مؤلفهٔ فرکانس بالا را از تفکیک پایین‌گذر می‌سازد، پوش آن را آشکار می‌کند و بالاتر از Threshold تضعیفش می‌کند. Range کاهش محاسباتی را محدود و Focus سهم آن را تنظیم می‌کند. این ابزار تشخیص گفتار آموزش‌دیده یا پردازندهٔ باندگذر جراحی با پهنای قابل‌تنظیم نیست.

Input → High-frequency split / detector → Range-limited attenuation → Mix → Output

### مرجع کنترل‌ها

#### Detection frequency / `frequency`

Range: 2000 Hz to 12000 Hz | Default: 6000 Hz

فرکانس تفکیک پایین‌گذر برای ساخت مؤلفهٔ بالا و آشکارساز است؛ مرکز یک باند باریک نیست.

Learn: موجود

#### Threshold / `threshold`

Range: -60 dB to 0 dB | Default: -24 dB

سطح پوش مؤلفهٔ بالا که پس از آن تضعیف محاسباتی بیشتر می‌شود؛ RMS کل ورودی نیست.

Learn: موجود

#### Range / `range`

Range: 0 dB to 18 dB | Default: 6 dB

بیشینهٔ کاهش محاسباتی باند بالا پیش از مقیاس Focus و Mix است. صفر مقدار کاهش را غیرفعال می‌کند.

Learn: موجود

#### Attack / `attack`

Range: 0.1 ms to 30 ms | Default: 1 ms

پاسخ زمانی پوش آشکارساز به افزایش انرژی بالا است.

فقط دستی؛ بدون Learn

#### Release / `release`

Range: 10 ms to 500 ms | Default: 80 ms

بازیابی پوش آشکارساز پس از جهش فرکانس بالا است.

Learn: موجود

#### Focus / `focus`

Range: 20% to 100% | Default: 60%

سهم تضعیف فرکانس بالا را مقیاس می‌کند. کنترل شدت است، نه Q یا پهنای آشکارساز.

Learn: موجود

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

صدای خشک را با شاخهٔ دارای کاهش فرکانسی ترکیب می‌کند.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین نهایی پس از ترکیب دی‌اسینگ است.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Sibilance / Focused
03. Vocal Sibilance / Soft
04. Vocal Sibilance / Open
05. Vocal Sibilance / Parallel
06. Bright Voice / Natural
07. Bright Voice / Focused
08. Bright Voice / Soft
09. Bright Voice / Open
10. Bright Voice / Parallel
11. Low Voice / Natural
12. Low Voice / Focused
13. Low Voice / Soft
14. Low Voice / Open
15. Low Voice / Parallel
16. Soft Speech / Natural
17. Soft Speech / Focused
18. Soft Speech / Soft
19. Soft Speech / Open
20. Soft Speech / Parallel
21. Cymbal Restraint / Natural
22. Cymbal Restraint / Focused
23. Cymbal Restraint / Soft
24. Cymbal Restraint / Open
25. Cymbal Restraint / Parallel
26. Acoustic Pick / Natural
27. Acoustic Pick / Focused
28. Acoustic Pick / Soft
29. Acoustic Pick / Open
30. Acoustic Pick / Parallel
31. Air Retention / Natural
32. Air Retention / Focused
33. Air Retention / Soft
34. Air Retention / Open
35. Air Retention / Parallel
36. Hard Consonants / Natural
37. Hard Consonants / Focused
38. Hard Consonants / Soft
39. Hard Consonants / Open
40. Hard Consonants / Parallel
41. Bus High Tamer / Natural
42. Bus High Tamer / Focused
43. Bus High Tamer / Soft
44. Bus High Tamer / Open
45. Bus High Tamer / Parallel
46. Parallel Silk / Natural
47. Parallel Silk / Focused
48. Parallel Silk / Soft
49. Parallel Silk / Open
50. Parallel Silk / Parallel

### استفادهٔ عملی

1. جمله‌ای شامل مصوت و صامت سیبیلانس‌دار را لوپ کنید و از حدود ۵ تا ۷ کیلوهرتز شروع کنید.
2. Range حدود ۳ تا ۶ دسی‌بل انتخاب و Threshold را تا کنترل صامت‌های قوی پایین بیاورید.
3. از Attack حدود ۱ و Release حدود ۶۰ تا ۱۲۰ میلی‌ثانیه شروع کنید و کدری یا حالت نوک‌زبانی را بشنوید.
4. اگر کل جمله به‌جای صداهای مسئله‌دار هوا و شفافیتش را از دست می‌دهد، Focus یا Mix را کاهش دهید.
5. بای‌پس را هم‌سطح مقایسه کنید و نفس‌های آرام و پایان حروف را هم بشنوید.

Frequency، Threshold، Range، Release و Focus دارای Learn هستند. انرژی باند بالا تشخیص و کاهش را هدایت، ضریب قله رهایی را برآورد و smooth آن را بلندتر می‌کند. Attack، Mix و Output دستی می‌مانند. تشخیص قطعی سیبیلانس از هر ترنزینت روشن غیرآوازی یا تطبیق وکال مرجع انجام نمی‌شود.

Detection Frequency به نقطهٔ تفکیک مربوط است، نه مرکز یک باند باریک و منزوی سیبیلانس. سنج، نفس و ساز روشن هم می‌توانند آن را تحریک کنند. عدد GR افت دقیق کل سیگنال نیست، چون Focus و Mix نیز نتیجه را تغییر می‌دهند.

در این بیلد دکمهٔ شنیدن Detector یا سایدچین خارجی وجود ندارد. پس از پیدا کردن مسئله در سولو، درون میکس تنظیم کنید. روی منبع استریو، به آشکارسازی مستقل دو کانال توجه کنید.

## DGate — گیت و کنترل سطح

DGate بالاتر از Threshold باز می‌شود، به مدت تعیین‌شده باز می‌ماند و سپس نرم به کف محدودشده با Range می‌بندد. Hysteresis آستانهٔ بستن را پایین‌تر می‌گذارد تا تق‌تق کاهش یابد. پردازندهٔ فعلی گیت است و نسبت متغیر اکسپنشن ندارد.

Stereo-linked peak detection → Threshold / hysteresis / hold → Smoothed gate gain → Output

### مرجع کنترل‌ها

#### Threshold / `threshold`

Range: -80 dB to 0 dB | Default: -45 dB

آستانهٔ بازشدنِ آشکارساز دامنهٔ لحظه‌ای با پیوند استریو است. افزایش آن صدای آرام بیشتری را رد می‌کند.

Learn: موجود

#### Attack / `attack`

Range: 0.1 ms to 100 ms | Default: 2 ms

زمان هموارسازی هنگام حرکت گین به وضعیت باز است. مقدار بسیار کوتاه ممکن است ناگهانی شنیده شود.

Learn: موجود

#### Hold / `hold`

Range: 0 ms to 500 ms | Default: 50 ms

مدت بازماندن پس از افت سطح زیر ناحیهٔ بستن است و به جلوگیری از بسته‌شدن چندباره در یک رویداد کمک می‌کند.

فقط دستی؛ بدون Learn

#### Release / `release`

Range: 5 ms to 2000 ms | Default: 150 ms

زمان هموارسازی حرکت به کف بسته است. مقدار بلندتر بخش بیشتری از دنباله را حفظ می‌کند.

Learn: موجود

#### Range / `range`

Range: 0 dB to 90 dB | Default: 60 dB

تضعیف وضعیت بسته است. صفر تضعیفی ندارد؛ مقدار بزرگ صدا را شدید کم می‌کند، اما مطلوب‌بودن آن را تشخیص نمی‌دهد.

فقط دستی؛ بدون Learn

#### Hysteresis / `hysteresis`

Range: 0 dB to 18 dB | Default: 6 dB

اختلاف آستانهٔ باز و بسته است. آستانهٔ بستن برابر Threshold منهای Hysteresis خواهد بود.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین پس از گیت است و در وضعیت باز هم اعمال می‌شود.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Cleanup / Focused
03. Vocal Cleanup / Soft
04. Vocal Cleanup / Open
05. Vocal Cleanup / Parallel
06. Kick Isolation / Natural
07. Kick Isolation / Focused
08. Kick Isolation / Soft
09. Kick Isolation / Open
10. Kick Isolation / Parallel
11. Snare Isolation / Natural
12. Snare Isolation / Focused
13. Snare Isolation / Soft
14. Snare Isolation / Open
15. Snare Isolation / Parallel
16. Tom Decay / Natural
17. Tom Decay / Focused
18. Tom Decay / Soft
19. Tom Decay / Open
20. Tom Decay / Parallel
21. Guitar Noise / Natural
22. Guitar Noise / Focused
23. Guitar Noise / Soft
24. Guitar Noise / Open
25. Guitar Noise / Parallel
26. Bass Tail / Natural
27. Bass Tail / Focused
28. Bass Tail / Soft
29. Bass Tail / Open
30. Bass Tail / Parallel
31. Speech Pause / Natural
32. Speech Pause / Focused
33. Speech Pause / Soft
34. Speech Pause / Open
35. Speech Pause / Parallel
36. Percussion Tight / Natural
37. Percussion Tight / Focused
38. Percussion Tight / Soft
39. Percussion Tight / Open
40. Percussion Tight / Parallel
41. Room Reduction / Natural
42. Room Reduction / Focused
43. Room Reduction / Soft
44. Room Reduction / Open
45. Room Reduction / Parallel
46. Gentle Floor / Natural
47. Gentle Floor / Focused
48. Gentle Floor / Soft
49. Gentle Floor / Open
50. Gentle Floor / Parallel

### استفادهٔ عملی

1. به‌جای تضعیف بیشینه، از Range متوسط مانند ۱۲ تا ۲۴ دسی‌بل شروع کنید.
2. Threshold را کمی پایین‌تر از آرام‌ترین رویداد مطلوب و بالاتر از نشت ناخواسته تنظیم کنید.
3. برای ساز کوبه‌ای Attack حدود ۱ تا ۵ میلی‌ثانیه را امتحان و Hold را طوری تنظیم کنید که یک ضربه چند بار باز نشود.
4. Release را تا محوشدن طبیعی دنباله بیشتر کنید؛ اگر گیت تق‌تق می‌کند، Hysteresis اضافه کنید.
5. همهٔ بخش‌های آرام را بررسی کنید؛ برای وکال بسیار عریان، ویرایش دستی ممکن است از گیت شدید امن‌تر باشد.

Threshold، Attack و Release دارای Learn هستند. آستانه از RMS ورودی منهای ۱۸ دسی‌بل می‌آید، نه کف نویز اندازه‌گیری‌شده؛ زمان‌ها از ضریب قله کمک می‌گیرند و smooth رهایی را بلندتر می‌کند. Range، Hold، Hysteresis و Output دستی می‌مانند. صداهای آرام مطلوب و دنباله‌ها را بررسی کنید.

گیت نمی‌تواند نویز زیر صدای مطلوب را جدا کند. یادگیرنده توزیع مستقل کف نویز را اندازه نمی‌گیرد. واژه‌های آرام، ضربه‌های ظریف و دنباله‌ها ممکن است قطع شوند؛ پیش از پذیرش پیشنهاد، آرام‌ترین بخش مطلوب را بشنوید.

آشکارساز مقدار لحظه‌ای بزرگ‌ترِ چپ یا راست را دنبال می‌کند؛ بنابراین هر کانال می‌تواند گیت را باز کند. Range بزرگ الزاماً سکوت دیجیتال نیست. سایدچین خارجی، فیلتر Key یا کنترل Look-ahead وجود ندارد.

## DFX — مولتی‌افکت ماژولار

DFX سچوریشن، فیلتر پایین‌گذر، دیلی و شبکهٔ فضای مستقل را با ترتیب ثابت به هم وصل می‌کند. هر ماژول فعال‌سازی مستقل دارد؛ فقط Saturation، Filter و Ambience دارای دامنهٔ Learn هستند. Delay Mix و Space Mix ترکیب محلی و Mix اصلی ترکیب کل زنجیره با ورودی اولیه را تعیین می‌کنند.

Input → Saturation → Filter → Delay → Ambience → Master mix → Output

### مرجع کنترل‌ها

#### Enable saturation / `driveOn`

Range: Off / On | Default: On

طبقهٔ سچوریشن tanh را وارد یا بای‌پس می‌کند. این کنترل دستی است؛ یادگیری وضعیت فعال‌سازی را تغییر نمی‌دهد.

فقط دستی؛ بدون Learn

#### Drive / `drive`

Range: 0 dB to 24 dB | Default: 3 dB

گین ورودی سچوریشن در حالت فعال است. انتخاب مستقل کاراکتر یا اورسمپلینگ ندارد.

Learn: موجود

#### Enable filter / `filterOn`

Range: Off / On | Default: On

طبقهٔ پایین‌گذر ورودی را فعال می‌کند. خاموش‌کردن آن فیلتر بازخورد دیلی را حذف نمی‌کند.

فقط دستی؛ بدون Learn

#### Low pass / `filter`

Range: 200 Hz to 18000 Hz | Default: 12000 Hz

فرکانس قطع پایین‌گذر برای ماژول فیلتر و مسیر بازخورد دیلی است.

Learn: موجود

#### Enable delay / `delayOn`

Range: Off / On | Default: On

ترکیب محلی اکو را وارد می‌کند. هنگام بای‌پس سهم خروجی، حافظهٔ دیلی ممکن است همچنان کار کند.

فقط دستی؛ بدون Learn

#### Time / `time`

Range: 1 ms to 2000 ms | Default: 250 ms

زمان اسمی دیلی بر حسب میلی‌ثانیه است؛ همگام‌سازی BPM میزبان وجود ندارد.

فقط دستی؛ بدون Learn

#### Feedback / `feedback`

Range: 0 (0%) to 0.9 (90%) | Default: 0.3 (30%)

مقدار بازگردش با سقف ۰٫۹۰ است. مقدار زیاد می‌تواند دنبالهٔ بلند و پرسطح بسازد.

فقط دستی؛ بدون Learn

#### Delay mix / `delayMix`

Range: 0% to 100% | Default: 20%

ترکیب ورودی ماژول با سیگنال تأخیری، پیش از Ambience است.

فقط دستی؛ بدون Learn

#### Enable ambience / `reverbOn`

Range: Off / On | Default: On

سهم فضای مستقل را وارد می‌کند و بای‌پس اصلی نیست.

فقط دستی؛ بدون Learn

#### Decay / `decay`

Range: 0.1 s to 8 s | Default: 1.5 s

طرح میرایی بازخوردِ شبکهٔ فضای با کاراکتر ثابت است. دیگر کنترل‌های ریورب در DFX در دسترس نیستند.

Learn: موجود

#### Ambience mix / `reverbMix`

Range: 0% to 100% | Default: 20%

ترکیب محلی خشک و فضا پس از دیلی است. «خشک» در این نقطه ورودی ماژول فضاست، نه الزاماً ورودی اولیهٔ پلاگین.

فقط دستی؛ بدون Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 50%

ترکیب کلی ورودی اولیه با زنجیرهٔ کامل افکت، پیش از Output است.

فقط دستی؛ بدون Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

گین نهایی پس از ترکیب اصلی است و حتی در Mix صفر هم اعمال می‌شود.

فقط دستی؛ بدون Learn

### ۵۰ پریست کارخانه

01. Init / Factory Default
02. Vocal Halo / Focused
03. Vocal Halo / Soft
04. Vocal Halo / Open
05. Vocal Halo / Parallel
06. Drum Space / Natural
07. Drum Space / Focused
08. Drum Space / Soft
09. Drum Space / Open
10. Drum Space / Parallel
11. Guitar Trail / Natural
12. Guitar Trail / Focused
13. Guitar Trail / Soft
14. Guitar Trail / Open
15. Guitar Trail / Parallel
16. Piano Cloud / Natural
17. Piano Cloud / Focused
18. Piano Cloud / Soft
19. Piano Cloud / Open
20. Piano Cloud / Parallel
21. Bass Texture / Natural
22. Bass Texture / Focused
23. Bass Texture / Soft
24. Bass Texture / Open
25. Bass Texture / Parallel
26. Synth Orbit / Natural
27. Synth Orbit / Focused
28. Synth Orbit / Soft
29. Synth Orbit / Open
30. Synth Orbit / Parallel
31. LoFi Room / Natural
32. LoFi Room / Focused
33. LoFi Room / Soft
34. LoFi Room / Open
35. LoFi Room / Parallel
36. Dark Cinematic / Natural
37. Dark Cinematic / Focused
38. Dark Cinematic / Soft
39. Dark Cinematic / Open
40. Dark Cinematic / Parallel
41. Percussion Spark / Natural
42. Percussion Spark / Focused
43. Percussion Spark / Soft
44. Percussion Spark / Open
45. Percussion Spark / Parallel
46. Ambient Wash / Natural
47. Ambient Wash / Focused
48. Ambient Wash / Soft
49. Ambient Wash / Open
50. Ambient Wash / Parallel

### استفادهٔ عملی

1. از Mix اصلی کم و Drive ملایم شروع کنید و هنگام شکل‌دادن به رنگ، Output را نزدیک بهرهٔ واحد نگه دارید.
2. Filter را فعال و فرکانسی انتخاب کنید که وضوح منبع حفظ شود.
3. Time را دستی تنظیم و پیش از افزودن فضا، Feedback و Delay Mix را متوسط نگه دارید.
4. Ambience را با Decay کوتاه فعال کنید و Space Mix را فقط به اندازهٔ لازم بالا ببرید.
5. برای تغییر متمرکز از Learn ماژول استفاده کنید؛ سپس سطح کل زنجیره را برابر و دنباله را بررسی کنید.

فقط Drive، Filter و Decay در گروه‌های Saturation، Filter و Ambience دارای Learn هستند. ماژول Delay کاملاً دستی است. کلیدهای فعال‌سازی، Time، Feedback، ترکیب‌های محلی و اصلی و Output در تمام دامنه‌های یادگیری حفظ می‌شوند. LEARN ALL بهینه‌ساز مشترک زنجیره، لادنس یا خطای مرجع نیست.

در این بیلد ترتیب مسیر عوض نمی‌شود. خاموش‌کردن ماژول الزاماً بافر آن را پاک نمی‌کند. فرکانس Filter حتی وقتی ماژول فیلتر ورودی خاموش است، در فیلتر بازخورد دیلی هم نقش دارد. ماژول مدولاسیون مستقل اضافه وجود ندارد.

در Mix اصلی صفر، ورودی اولیه همچنان از گین Output عبور می‌کند. در صددرصد هم ممکن است به‌علت ترکیب محلی دیلی و ریورب، مؤلفهٔ خشک وجود داشته باشد. برای حذف کامل نمونه از بای‌پس میزبان استفاده کنید.

## جلسه، اتومیشن و بازخوانی

### چه چیزی ذخیره می‌شود

پردازنده برای بازخوانی میزبان، وضعیت پارامتر را با APVTS/XML ذخیره می‌کند. پس از پذیرش تنظیم، پروژه را ذخیره کنید. بازخوانی و مهاجرت وضعیت قدیمی هنوز نیازمند اعتبارسنجی میزبان است؛ باینری قبلی را با آرشیو نگه دارید. هر پلاگین ۵۰ برنامهٔ کارخانه و مرورگر بالای رابط دارد؛ ستاره ویرایش را نشان می‌دهد و وضعیت میزبان مقدار واقعی را نگه می‌دارد. حافظهٔ A/B و ورود و خروج فایل پریست اختصاصی وجود ندارد. بارگذاری برنامه، یادگیری و پیش‌شنوی معلق را لغو می‌کند.

صوت مرجع، توصیف، فهرست پیشنهاد و Undo یک‌سطحی در وضعیت پردازنده جاسازی نمی‌شوند. پس از بستن رابط یا بازکردن پروژه، مرجع را دوباره بارگذاری کنید. در صورت مجازبودن نسخه‌ای در دارایی پروژه نگه دارید؛ نمایش نام فایل به معنی ذخیره‌شدن آن در جلسه نیست.

### روش امن اتومیشن

پارامترها با شناسه‌های ثابت مرجع کنترل به میزبان معرفی می‌شوند. اتومیشن میزبان با Undo پیشنهاد جداست. تغییر Learn به میزبان خبر داده می‌شود، پس حالت خواندن یا نوشتن میزبان می‌تواند اثر بگذارد. پیش از Learn نوشتن اتومیشن را متوقف و نتیجه را روی کپی آزمایش کنید. حالت‌های گسسته را مانند مقدار پیوسته جاروب نکنید.

### فریز و خروجی‌گیری

در رندر آفلاین برای دنبالهٔ ریورب و دیلی زمان بگذارید. پردازنده برای DVerb، DDelay و DFX دنبالهٔ ۳۰ثانیه‌ای به میزبان اعلام می‌کند؛ این راهنماست، نه تضمین اندازه‌گیری‌شدهٔ هر تنظیم. مرز و فید را کنترل کنید. تأخیر جبرانی اضافه صریحاً اعلام نشده؛ پیش‌تأخیر و اکوی شنیدنی عمدی است و با PDC یکی نیست.

## عیب‌یابی و پرسش‌های رایج

### پلاگین در میزبان دیده نمی‌شود

سیستم‌عامل، معماری، فرمت و مسیر اسکن را بررسی کنید. مطمئن شوید باینری واقعی نصب شده، نه ZIP طراحی یا پوشهٔ سورس. طبق راهنمای میزبان دوباره اسکن و بازگشایی کنید. میزبان صرفاً AAX به محصول AAX نیاز دارد، نه این اهداف VST3/AU. فایل اجرایی داخل بسته را تغییر نام ندهید.

### صوت یا نتیجهٔ یادگیری ندارم

مسیر، Mute ترک، بای‌پس میزبان، دستگاه، گین خروجی و رسیدن پخش به نمونه را بررسی کنید. اگر گیت صدای مطلوب را می‌بندد، Threshold یا Range را کم کنید. برای Learn پخش را طی دریافت ادامه دهید، از سکوت دوری و حالت یا مرجع معتبر انتخاب کنید. عوض‌شدن برچسب به‌تنهایی پیشنهاد معتبر نیست.

### مرجع رد شده یا از دست رفته است

یک WAV محلی PCM قابل‌خواندن یا AIFF/FLAC پشتیبانی‌شده، نرخ معتبر و ابتدای غیرساکت امتحان کنید. فقط صوت مجاز را تبدیل کنید. خواننده ۳۰ ثانیهٔ نخست را تحلیل می‌کند، نه بخش انتخابیِ بعدتر. پس از بازخوانی رابط یا پروژه بارگذاری دوباره لازم است، چون مرجع ذخیره نمی‌شود.

### پیشنهاد ناپدید شده یا اعمال نمی‌شود

ویرایش دستی یا میزبان می‌تواند پیشنهاد را نامعتبر کند. پس از پایان ویرایش دوباره Learn کنید. تحلیل‌های هم‌پوشان نخواهید؛ دریافت مشغول یا زمان‌تمام‌شده شاید برای پاک‌شدن به پخش نیاز داشته باشد. UNDO فقط یک عمل یادگیری اعمال‌شده را برمی‌گرداند، نه کل تاریخچهٔ میزبان.

### کلیک، اورلود یا نوسان بلندی

مانیتور و گین خروجی را کم کنید. Drive، تقویت EQ، Makeup و بازخورد را بررسی کنید. هم‌سطح مقایسه، هد‌روم حفظ و در صورت نیاز لیمیتر ایمنی مستقل استفاده کنید. هنگام فشار پردازشی بافر میزبان را بیشتر و نرخ یا تعداد نمونهٔ غیرضروری را کم کنید. هموارسازی پارامتر، بی‌آرتیفکت‌بودن تغییر حالت یا اتومیشن شدید را تضمین نمی‌کند.

### چرا با Mono هنوز استریو می‌شنوم؟

در DStereo، Mono بر شاخهٔ پردازش‌شده اثر دارد. برای بررسی کامل Mix را صددرصد یا مانیتور مونو میزبان را فعال کنید. Width از کانال‌های یکسان Side نمی‌سازد. همبستگی منفی به شنیدن و تشخیص نیاز دارد، نه فقط افزایش Center یا اعتماد به عدد.

### آیا AI به معنی مدل آموزش‌دیده است؟

در این پیاده‌سازی نه. مسیر موجود از تحلیل قطعی ویژگی و هدف‌های ابتکاری استفاده می‌کند. وزن مدل، استنتاج راه‌دور یا سرویس تطبیق عصبی فعال نیست. رابط انتزاعی مدل آینده، قابلیت محصول آموزش‌دیده نیست. توضیح پیشنهاد و محدودیت پارامتر را بخوانید.

## مرجع فنی و محدودیت‌های شناخته‌شده

### معماری صوتی

مسیر AudioBuffer در JUCE نمونه‌های float را با وضعیت داخلی دوبرابر برای بای‌کواد EQ پردازش می‌کند. چیدمان مونو یا استریوی همسان است؛ MIDI و باس سایدچین خارجی پیاده‌سازی نشده‌اند. هسته نرخ آماده‌سازی را به ۸ تا ۳۸۴ کیلوهرتز محدود می‌کند؛ این حد پیاده‌سازی است، نه تأیید کامل نرخ‌های میزبان. برخی فرکانس‌های قطع در داخل زیر نایکوئیست محدود می‌شوند؛ پس عدد بالای نمایشگر در نرخ پایین الزاماً فرکانس مؤثر نیست.

مقادیر پیوسته در هسته با ضریب حدود ۲۰ میلی‌ثانیه هموار و انتخاب گسسته فوری عوض می‌شود. بافر دیلی و FDN در آماده‌سازی تخصیص می‌یابد. تحلیل و رمزگشایی وظیفهٔ Worker است و عمداً در Callback صوتی قرار ندارد. نمایش از صف محدود تک‌تولیدکننده و تک‌مصرف‌کننده استفاده می‌کند و می‌تواند فریم نمایشی را کنار بگذارد.

### محدودیت عملی

اورسمپلینگ، Look-ahead، لیمیتر True Peak، تشخیص تمپو از صوت، کانولوشن دلخواه، فهم عمومی زبان یا بهینه‌ساز تکراری خروجی و مرجع وجود ندارد. DEQ فاز خطی، DSess اکولایزر داینامیک پارامتریک کامل و DGate دارای نسبت اکسپنشن نیست. ترتیب DFX ثابت است. کیفیت و اتومیشن باید با منابع واقعی شنیده شوند.

مصرف پردازنده و حافظه به نرخ، پلتفرم، تعداد نمونه و تنظیمات وابسته است. حداقل اندازه‌گیری‌شدهٔ RAM/CPU یا تضمین بافر منتشر نشده است. محافظت مقدار متناهی و محدودسازی داخلی خروجی، جلوگیری شفاف از اورلود نیست. از متر میزبان و سطح ورودی معقول استفاده کنید.

### وضعیت اعتبارسنجی

تست‌های قابل‌حمل DSP و یادگیری و کنترل چیدمان و صف وجود دارند؛ این‌ها تست شنیداری یا تأیید همهٔ DAWها نیستند. چک‌پوینت قبلی بیلد CI سه پلتفرم را پاس کرده؛ رابط جدید اعتبارسنجی بومی و میزبان جدا دارد. شواهد همان کامیت را در Checks و STATUS.md ببینید. دفترچه گواهی انتشار نیست.

## واژه‌نامه

**حمله** — پاسخ افزایش پوش؛ در DTrans گین حمله است، نه زمان.

**مسیر کمکی / ارسال** — باس موازی میزبان که مقدار معینی از سیگنال را می‌گیرد.

**ضریب قله** — نسبت قله به RMS که معمولاً دسی‌بلی بیان می‌شود.

**دسی‌بل تمام‌مقیاس** — سطح نسبت به تمام‌مقیاس دیجیتال؛ واحد لادنس نیست.

**خشک / افکتی** — سیگنال اصلی در برابر شاخهٔ پردازش‌شده.

**بازخورد** — بازگرداندن بخشی از سیگنال تأخیری یا فضایی به مسیر ورودی‌اش.

**شبکهٔ تأخیر بازخوردی** — خطوط تأخیر بازخوردی متصل برای ساخت فضای الگوریتمی.

**کاهش گین** — تضعیف محاسبه‌شده در پردازش داینامیک.

**هد‌روم** — فاصلهٔ قلهٔ فعلی تا مرز اورلود موردنظر.

**هیسترزیس** — آستانه‌های متفاوت باز و بسته برای کاهش قطع و وصل سریع.

**زانو** — ناحیهٔ گذار میان رفتار بدون کمپرس و کمپرس.

**میانی / جانبی** — مؤلفه‌های مجموع و تفاضل سیگنال استریو.

**سازگاری مونو** — رفتار سیگنال هنگام جمع کانال‌ها.

**جبران تأخیر پلاگین** — جبران میزبان برای تأخیر پردازشی اعلام‌شده.

**پیش‌تأخیر** — فاصله تا شروع برگشت یک افکت زمانی.

**ضریب کیفیت** — گزینش‌پذیری فیلتر Bell؛ Q بالاتر یعنی پهنای باریک‌تر.

**رهایی** — هموارسازی بازیابی یا بستن پس از رویداد سطح.

**ریشهٔ میانگین مربعات** — اندازهٔ دامنه بر پایهٔ انرژی؛ نه لادنس LUFS.

**زمان افت ۶۰ دسی‌بل** — زمان افت اندازه‌گیری‌شدهٔ ۶۰ دسی‌بل؛ خوانش تأییدشدهٔ DVerb نیست.

**سیبیلانس** — انرژی بالای قوی گفتار مانند صداهای س و ش.

**همبستگی استریو** — رابطهٔ موج کانال‌ها؛ مقدار منفی نیازمند بررسی مونو است.

**قلهٔ بین‌نمونه‌ای** — برآورد قلهٔ بازسازی‌شده میان نمونه‌ها؛ مترهای فعلی آن را نمی‌سنجند.

## پشتیبانی، حریم خصوصی و ملاحظات حقوقی

### گزارش قابل‌بازتولید

از Issue مخزن یا کانال همراه بیلد استفاده کنید. نام پلاگین، بیلد یا کامیت، فرمت، سیستم و پردازنده، نسخهٔ DAW، نرخ، بافر، چیدمان، مراحل، انتظار و نتیجه و اثر بای‌پس را ذکر کنید. لاگ کرش را پس از بررسی مسیر و شناسهٔ خصوصی بفرستید.

جلسهٔ خصوصی، موسیقی مرجع دارای حق، اطلاعات ورود یا دادهٔ شخصی منتشر نکنید. فایل کوتاه مجازِ مصنوعی یا ناشناس و پروژهٔ حداقلی بهتر است. Issue Tracker قرارداد سطح خدمت، وعدهٔ زمان پاسخ یا ضمانت نیست.

### پردازش محلی و حقوق استفاده

کد توصیف‌شده مرجع را محلی رمزگشایی و ورودی را تحلیل می‌کند؛ مسیر بارگذاری برای یادگیری یا استنتاج راه‌دور پیاده‌سازی نشده است. این گفته ممیزی مستقل امنیت یا سیاست DAW، سیستم، پشتیبان ابری و ابزار ثالث نیست. حقوق منبع و مرجع را رعایت کنید؛ تحلیل مجوز تکثیر یا توزیع نمی‌دهد.

### مجوز و دامنهٔ سند

اعلان مخزن، سورس نمونهٔ اولیه را برای ارزیابی و توسعه معرفی می‌کند. این راهنما جایگزین مجوز نرم‌افزار نیست، حق توزیع تجاری نمی‌دهد و سیاست بازپرداخت یا ضمانت نمی‌سازد. پیش از توزیع، مجوز JUCE را جدا بررسی کنید. فونت‌های همراه اعلان SIL Open Font License خود را دارند.

نام و آثار D.A Plugins این پروژه را معرفی می‌کنند. نام محصولات یا میزبان‌های مربوط به سازگاری متعلق به صاحبانشان است و تأیید ثالث را القا نمی‌کند. این متن اصیل است و از دفترچهٔ سازندهٔ دیگر کپی نشده. نسخهٔ انگلیسی و فارسی همان مجموعهٔ قابلیت توسعه‌ای را توضیح می‌دهند.
